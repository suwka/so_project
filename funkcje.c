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

// Globalne zmienne do obsługi IPC
int semafor, kolejka, pamiec_kasy;
Kasa *kasa;

// Nominały używane w systemie
const int NOMINALY[LICZBA_NOMINALOW] = {10, 20, 50};

// Funkcja inicjalizująca zasoby współdzielone
void inicjalizuj_zasoby() {
    // Tworzenie klucza IPC za pomocą ftok()
    key_t klucz = ftok("/tmp", 'S');

    // Tworzenie zestawu semaforów (5 semaforów)
    semafor = semget(klucz, 5, IPC_CREAT | 0666);
    if (semafor == -1) {
        perror("Blad semafora");
        exit(EXIT_FAILURE);
    }

    // Tworzenie kolejki komunikatów
    kolejka = msgget(klucz, IPC_CREAT | 0666);
    if (kolejka == -1) {
        perror("Blad kolejki");
        exit(EXIT_FAILURE);
    }

    // Tworzenie segmentu pamięci dzielonej
    pamiec_kasy = shmget(klucz, sizeof(Kasa), IPC_CREAT | 0666);
    if (pamiec_kasy == -1) {
        perror("Blad pamieci wspoldzielonej");
        exit(EXIT_FAILURE);
    }

    // Dołączenie segmentu pamięci dzielonej
    kasa = (Kasa *)shmat(pamiec_kasy, NULL, 0);
    if (kasa == (void *)-1) {
        perror("Blad dolaczenia pamieci wspoldzielonej");
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja początkowych wartości banknotów w kasie
    int startowe_banknoty[LICZBA_NOMINALOW] = KASA_STARTOWA_BANKNOTY;
    for (int i = 0; i < LICZBA_NOMINALOW; i++) {
        kasa->banknoty[i] = startowe_banknoty[i];
    }

    // Inicjalizacja wartości semaforów
    // Semafor 0: zarządzanie miejscami w poczekalni
    semctl(semafor, 0, SETVAL, POCZEKALNIA);
    // Semafor 1: zarządzanie dostępnością foteli fryzjerskich
    semctl(semafor, 1, SETVAL, FOTELE);
    // Semafor 2: synchronizacja klient-fryzjer (gotowość do strzyżenia)
    semctl(semafor, 2, SETVAL, 0);
    // Semafor 3: zapewnia dostęp do zasobów współdzielonych (kasa)
    semctl(semafor, 3, SETVAL, 1);
    // Semafor 4: blokada przychodzenia klientów podczas ewakuacji
    semctl(semafor, 4, SETVAL, 1);
}

// Funkcja zwalniająca zasoby IPC
void zwolnij_zasoby() {
    // Usunięcie zestawu semaforów
    semctl(semafor, 0, IPC_RMID);
    // Usunięcie kolejki komunikatów
    msgctl(kolejka, IPC_RMID, NULL);
    // Odłączenie pamięci dzielonej
    shmdt(kasa);
    // Usunięcie segmentu pamięci dzielonej
    shmctl(pamiec_kasy, IPC_RMID, NULL);
    exit(0);
}

// Funkcja inicjalizująca zasoby klienta
void inicjalizuj_klienta(Klient *klient) {
    // Inicjalizacja początkowych banknotów klienta
    int startowe_banknoty[LICZBA_NOMINALOW] = KLIENT_STARTOWE_BANKNOTY;
    for (int i = 0; i < LICZBA_NOMINALOW; i++) {
        klient->banknoty[i] = startowe_banknoty[i];
    }
}

// Funkcja obliczająca sumę wartości banknotów
int oblicz_sume_banknotow(const int *banknoty) {
    int suma = 0;
    for (int i = 0; i < LICZBA_NOMINALOW; i++) {
        suma += banknoty[i] * NOMINALY[i];
    }
    return suma;
}

// Funkcja dodająca banknoty do innego zbioru
void dodaj_banknoty(int *cel, const int *zrodlo) {
    for (int i = 0; i < LICZBA_NOMINALOW; i++) {
        cel[i] += zrodlo[i];
    }
}

// Funkcja obsługująca płatności klienta
int zaplac(int *zrodlo, int kwota, int *cel) {
    int suma_klienta = oblicz_sume_banknotow(zrodlo);
    if (suma_klienta < kwota) {
        printf(KASA_COLOR "Niewystarczające środki. Klient posiada: " VALUE_COLOR "%d" KASA_COLOR ", potrzebuje: " VALUE_COLOR "%d\n" VALUE_COLOR, suma_klienta, kwota);
        return 0;
    }

    int reszta = suma_klienta - kwota;
    printf(KASA_COLOR "Kwota do zapłaty: " VALUE_COLOR "%d" KASA_COLOR ", suma klienta: " VALUE_COLOR "%d" KASA_COLOR ", reszta do wydania: " VALUE_COLOR "%d\n" VALUE_COLOR, kwota, suma_klienta, reszta);

    // Dodawanie wpłaty do kasy
    for (int i = LICZBA_NOMINALOW - 1; i >= 0; i--) {
        while (zrodlo[i] > 0 && suma_klienta > 0) {
            suma_klienta -= NOMINALY[i];
            zrodlo[i]--;
            kasa->banknoty[i]++;
        }
    }

    // Obsługa wydawania reszty
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
            sleep(1); // Oczekiwanie na dostępne nominały
        }
    }

    if (reszta > 0) {
        printf(KASA_COLOR "Nie udało się wydać pełnej reszty. Pozostała reszta: " VALUE_COLOR "%d\n" VALUE_COLOR, reszta);
        return 0;
    }

    printf(KASA_COLOR "Transakcja zakończona sukcesem. Reszta wydana w pełni.\n" VALUE_COLOR);
    return 1;
}

// Funkcja realizująca operacje semaforowe
void operacja_semaforowa(int semid, int semnum, int operacja) {
    struct sembuf operacja_semaf = {semnum, operacja, 0};
    if (semop(semid, &operacja_semaf, 1) == -1) { 
        perror("Błąd operacji semaforowej");
        fprintf(stderr, "Kod błędu: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

// Funkcja czyszcząca kolejkę komunikatów
void wyczysc_kolejke(int kolejka) {
    Wiadomosc wiad;
    while (msgrcv(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 0, IPC_NOWAIT) != -1) {
        // Opróżnianie kolejki
    }
    printf(KIEROWNIK_COLOR "Kolejka komunikatów została wyczyszczona.\n" VALUE_COLOR);
}
