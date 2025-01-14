#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "funkcje.h"
#include <sys/prctl.h>
#include <time.h>


void zakoncz_symulacje(int sig) {
    //printf("Zakończenie symulacji.\n");
    zwolnij_zasoby();
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, zakoncz_symulacje); // Dodanie obsługi SIGINT
    inicjalizuj_zasoby();

    pid_t pid_kierownika;

    int czas_symulacji = (CZAS_ZAMKNIECIA - CZAS_OTWARCIA) * GODZINA;
    printf("Symulacja czasu pracy salonu: %d godzin będzie trwać %d sekund.\n", 
           CZAS_ZAMKNIECIA - CZAS_OTWARCIA, czas_symulacji);
    sleep(3);
    time_t start = time(NULL);

    // Uruchamiamy proces kierownika
    if ((pid_kierownika = fork()) == 0) {
        strcpy(argv[0], "kierownik");
        proces_kierownika();
        exit(0);
    }

    // Uruchamiamy procesy fryzjerów
    for (int i = 0; i < FRYZJERZY; i++) {
        if (fork() == 0) {
            strcpy(argv[0], "fryzjer");
            proces_fryzjera();
            exit(0);
        }
    }

    // Uruchamiamy procesy klientów
    for (int i = 0; i < KLIENCI; i++) {
        if (fork() == 0) {
            strcpy(argv[0], "klient");
            proces_klienta();
            exit(0);
        }
    }

    // Czekamy na zakończenie symulacji
    while (difftime(time(NULL), start) < czas_symulacji) {
        sleep(1); // Sprawdzanie co sekundę
    }

    printf("Czas symulacji upłynął. Zakończono symulację z powodu zakończenia czasu pracy salonu.\n");
    kill(0, SIGINT); // Wysyłamy sygnał zakończenia do wszystkich procesów

    zwolnij_zasoby();
    return 0;
}

