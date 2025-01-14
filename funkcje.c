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
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>


int semafor, kolejka, pamiec_kasy;
Kasa *kasa;

const int NOMINALY[LICZBA_NOMINALOW] = {10, 20, 50};

void inicjalizuj_zasoby() {
    key_t klucz = ftok("/tmp", 'S');

    semafor = semget(klucz, 5, IPC_CREAT | 0666);
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
    // Semafor 4 - zapewnia blokowanie przychodzenia do salonu w trakcie ewakuacji

    semctl(semafor, 0, SETVAL, POCZEKALNIA);
    semctl(semafor, 1, SETVAL, FOTELE);
    semctl(semafor, 2, SETVAL, 0);
    semctl(semafor, 3, SETVAL, 1);
    semctl(semafor, 4, SETVAL, 1);
}

void zwolnij_zasoby() {
    semctl(semafor, 0, IPC_RMID);
    msgctl(kolejka, IPC_RMID, NULL);
    shmdt(kasa);
    shmctl(pamiec_kasy, IPC_RMID, NULL);
    //printf("Zasoby zwolnione.\n");
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
        printf(KASA_COLOR "Niewystarczające środki. Klient posiada: " VALUE_COLOR "%d" KASA_COLOR ", potrzebuje: " VALUE_COLOR "%d\n" VALUE_COLOR, suma_klienta, kwota);
        return 0;
    }

    int reszta = suma_klienta - kwota;
    printf(KASA_COLOR "Kwota do zapłaty: " VALUE_COLOR "%d" KASA_COLOR ", suma klienta: " VALUE_COLOR "%d" KASA_COLOR ", reszta do wydania: " VALUE_COLOR "%d\n" VALUE_COLOR, kwota, suma_klienta, reszta);

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
            printf(KASA_COLOR "Brak odpowiednich nominałów w kasie. Oczekiwanie na dostępne banknoty.\n" VALUE_COLOR);
            sleep(1); 
        }
    }

    if (reszta > 0) {
        printf(KASA_COLOR "Nie udało się wydać pełnej reszty. Pozostała reszta: " VALUE_COLOR "%d\n" VALUE_COLOR, reszta);
        return 0;
    }

    printf(KASA_COLOR "Transakcja zakończona sukcesem. Reszta wydana w pełni.\n" VALUE_COLOR);
    return 1;
}

#define VALUE_COLOR "\033[0m"
#define VALUE_COLOR "\033[0m"

void operacja_semaforowa(int semid, int semnum, int operacja) {
    struct sembuf operacja_semaf = {semnum, operacja, 0};
    if (semop(semid, &operacja_semaf, 1) == -1) {
        perror("Błąd operacji semaforowej");
        fprintf(stderr, "Kod błędu: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

void wyczysc_kolejke(int kolejka) {
    Wiadomosc wiad;
    while (msgrcv(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 0, IPC_NOWAIT) != -1) {
        // Czyszczenie kolejki
    }
    printf(KIEROWNIK_COLOR "Kolejka komunikatów została wyczyszczona.\n" VALUE_COLOR);
}

