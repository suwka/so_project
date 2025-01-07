#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string.h>
#include "funkcje.h"
#include <sys/prctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/prctl.h>


int semafor, kolejka, pamiec_kasy;
Kasa *kasa;

const int NOMINALY[LICZBA_NOMINALOW] = {10, 20, 50};

void inicjalizuj_zasoby() {
    key_t klucz = ftok("/tmp", 'S');

    semafor = semget(klucz, 4, IPC_CREAT | 0666);
    if (semafor == -1) {
        perror("Blad semafora");
        exit(EXIT_FAILURE);
    }

    kolejka = msgget(klucz, IPC_CREAT | 0666);
    if (kolejka == -1) {
        perror("Blad kolejki");
        exit(EXIT_FAILURE);
    }

    pamiec_kasy = shmget(klucz, sizeof(Kasa), IPC_CREAT | 0666);
    if (pamiec_kasy == -1) {
        perror("Blad pamieci wspoldzielonej");
        exit(EXIT_FAILURE);
    }

    kasa = (Kasa *)shmat(pamiec_kasy, NULL, 0);
    if (kasa == (void *)-1) {
        perror("Blad dolaczenia pamieci wspoldzielonej");
        exit(EXIT_FAILURE);
    }

    int startowe_banknoty[LICZBA_NOMINALOW] = KASA_STARTOWA_BANKNOTY;
    for (int i = 0; i < LICZBA_NOMINALOW; i++) {
        kasa->banknoty[i] = startowe_banknoty[i];
    }

    // Semafor 0 - zarządzanie dostępnością miejsc w poczekalni
    // Semafor 1 - zarządzanie dostępnością foteli fryzjerskich
    // Semafor 2 - synchronizuje komunikację między klientem a fryzjerem (gotowość do strzyżenia).
    // Semafor 3 - zapewnia wyłączny dostęp do zasobów współdzielonych (kasa)

    semctl(semafor, 0, SETVAL, POCZEKALNIA);
    semctl(semafor, 1, SETVAL, FOTELE);
    semctl(semafor, 2, SETVAL, 0);
    semctl(semafor, 3, SETVAL, 1);
}

void zwolnij_zasoby() {
    semctl(semafor, 0, IPC_RMID);
    msgctl(kolejka, IPC_RMID, NULL);
    shmdt(kasa);
    shmctl(pamiec_kasy, IPC_RMID, NULL);
    printf("Zasoby zwolnione.\n");
    exit(0);
}

void inicjalizuj_klienta(Klient *klient) {
    int startowe_banknoty[LICZBA_NOMINALOW] = KLIENT_STARTOWE_BANKNOTY;
    for (int i = 0; i < LICZBA_NOMINALOW; i++) {
        klient->banknoty[i] = startowe_banknoty[i];
    }
}

int oblicz_sume_banknotow(const int *banknoty) {
    int suma = 0;
    for (int i = 0; i < LICZBA_NOMINALOW; i++) {
        suma += banknoty[i] * NOMINALY[i];
    }
    return suma;
}

void dodaj_banknoty(int *cel, const int *zrodlo) {
    for (int i = 0; i < LICZBA_NOMINALOW; i++) {
        cel[i] += zrodlo[i];
    }
}

int zaplac(int *zrodlo, int kwota, int *cel) {
    int suma_klienta = oblicz_sume_banknotow(zrodlo);
    if (suma_klienta < kwota) {
        printf("Niewystarczające środki. Klient posiada: %d, potrzebuje: %d\n", suma_klienta, kwota);
        return 0;
    }

    int reszta = suma_klienta - kwota;
    printf("Kwota do zapłaty: %d, suma klienta: %d, reszta do wydania: %d\n", kwota, suma_klienta, reszta);

    for (int i = LICZBA_NOMINALOW - 1; i >= 0; i--) {
        while (zrodlo[i] > 0 && suma_klienta > 0) {
            suma_klienta -= NOMINALY[i];
            zrodlo[i]--;
            kasa->banknoty[i]++;
        }
    }

    while (reszta > 0) {
        int wydano_reszte = 0;
        for (int i = LICZBA_NOMINALOW - 1; i >= 0; i--) {
            if (reszta >= NOMINALY[i] && kasa->banknoty[i] > 0) {
                reszta -= NOMINALY[i];
                kasa->banknoty[i]--;
                zrodlo[i]++;
                wydano_reszte = 1;
                break;
            }
        }

        if (!wydano_reszte) {
            //printf("Brak odpowiednich nominałów w kasie. Oczekiwanie na dostępne banknoty.\n");
            sleep(1); 
        }
    }

    if (reszta > 0) {
        printf("Nie udało się wydać pełnej reszty. Pozostała reszta: %d\n", reszta);
        return 0;
    }

    printf("Transakcja zakończona sukcesem. Reszta wydana w pełni.\n");
    return 1;
}

void operacja_semaforowa(int semid, int semnum, int operacja) {
    struct sembuf operacja_semaf = {semnum, operacja, 0};
    if (semop(semid, &operacja_semaf, 1) == -1) {
        perror("Błąd operacji semaforowej");
        exit(EXIT_FAILURE);
    }
}