#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include "funkcje.h"
#include <string.h>
#include <sys/prctl.h>
#include <errno.h>

void proces_kierownika() {
    srand(time(NULL));
    prctl(PR_SET_NAME, "kierownik", 0, 0, 0);
    while (1) {
        sleep(rand() % 10 + 5); // 5-15 sekund

        int akcja = rand() % 2; // 0 - zwolnienie fryzjera, 1 - ewakuacja klientów

        if (akcja == 0) {
            int losowy_fryzjer = rand() % FRYZJERZY;
            pid_t pid_fryzjera = getpid() + losowy_fryzjer + 1;
            if(kill(pid_fryzjera, SIGTERM) == -1) {
                perror("Błąd zwolnienia fryzjera");
                fprintf(stderr, "Kod błędu errno: %d, opis błędu %s\n", errno, strerror(errno));
                if (errno == ESRCH) {
                    fprintf(stderr, "Proces o PID %d nie istnieje.\n", pid_fryzjera);
                } else if (errno == EPERM) {
                    fprintf(stderr, "Brak uprawnień do wysłania sygnału do procesu o PID %d.\n", pid_fryzjera);
                }

            };
            printf("Kierownik: Zwolniono fryzjera o PID %d.\n", pid_fryzjera);
        } else {
            printf("Kierownik: Ewakuacja klientów!\n");
            for (int i = 0; i < KLIENCI; i++) {
                pid_t pid_klienta = getpid() + FRYZJERZY + i + 1;
                kill(pid_klienta, SIGUSR1); // Sygnał ewakuacji
            }
        }
    }
}


