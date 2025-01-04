#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "funkcje.h"

int main() {
    signal(SIGINT, zwolnij_zasoby);
    inicjalizuj_zasoby();

    pid_t pid_kierownika;

    // Uruchamiamy proces kierownika
    if ((pid_kierownika = fork()) == 0) {
        proces_kierownika();
        exit(0);
    }

    // Uruchamiamy procesy fryzjerów
    for (int i = 0; i < FRYZJERZY; i++) {
        if (fork() == 0) {
            proces_fryzjera();
            exit(0);
        }
    }

    // Uruchamiamy procesy klientów
    for (int i = 0; i < KLIENCI; i++) {
        if (fork() == 0) {
            proces_klienta();
            exit(0);
        }
    }

    // Oczekujemy na zakończenie wszystkich procesów
    for (int i = 0; i < FRYZJERZY + KLIENCI + 1; i++) { // +1 dla kierownika
        wait(NULL);
    }

    zwolnij_zasoby();
    return 0;
}
