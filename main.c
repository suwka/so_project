#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "funkcje.h"
#include <sys/prctl.h>


int main(int argc, char *argv[]) {
    signal(SIGINT, zwolnij_zasoby);
    inicjalizuj_zasoby();

    pid_t pid_kierownika;

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

    for (int i = 0; i < FRYZJERZY + KLIENCI + 1; i++) {
        wait(NULL);
    }

    zwolnij_zasoby();
    return 0;
}
