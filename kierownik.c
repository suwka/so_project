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


/*

problem kierownika jest taki ze zabija wszystkich klientow zamiast ich wypraszac z salonu - do rozwiazania

*/


void proces_kierownika() {
    srand(time(NULL));
    prctl(PR_SET_NAME, "kierownik", 0, 0, 0);
    while (1) {
        sleep(rand() % 10 + 5); // 5-15 sekund

        int akcja = rand() % 2; // 0 - zwolnienie fryzjera, 1 - ewakuacja klientów

        if (akcja == 0) {
            int losowy_fryzjer = rand() % FRYZJERZY;
            pid_t pid_fryzjera = getpid() + losowy_fryzjer + 1;
            kill(pid_fryzjera, SIGTERM);
            printf("Kierownik: Zwolniono fryzjera o PID %d.\n", pid_fryzjera);
        } else {
            printf("Kierownik: Ewakuacja klientów!\n");
            for (int i = 0; i < KLIENCI; i++) {
                pid_t pid_klienta = getpid() + FRYZJERZY + i + 1;
                kill(pid_klienta, SIGTERM);
                printf("Kierownik: Klient o PID %d został wyproszony.\n", pid_klienta);
            }
        }
    }
}
