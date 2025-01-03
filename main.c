#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "funkcje.h"

int main() {
    signal(SIGINT, zwolnij_zasoby);
    inicjalizuj_zasoby();

    for (int i = 0; i < FRYZJERZY; i++) {
        if (fork() == 0) {
            proces_fryzjera();
            exit(0);
        }
    }

    for (int i = 0; i < KLIENCI; i++) {
        if (fork() == 0) {
            proces_klienta();
            exit(0);
        }
    }

    for (int i = 0; i < FRYZJERZY + KLIENCI; i++) {
        wait(NULL);
    }

    zwolnij_zasoby();
    return 0;
}
