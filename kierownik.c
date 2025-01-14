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

volatile int aktywni_fryzjerzy = FRYZJERZY;

void proces_kierownika() {
    srand(time(NULL));
    prctl(PR_SET_NAME, "kierownik", 0, 0, 0);
    while (1) {
        sleep(rand() % 10 + 5);

        int akcja = rand() % 2; // 0 - zwolnienie fryzjera, 1 - ewakuacja klientów

        if (akcja == 0) {
            if (aktywni_fryzjerzy > 0) {
                int losowy_fryzjer = rand() % aktywni_fryzjerzy;
                pid_t pid_fryzjera = getpid() + losowy_fryzjer + 1;
                if (kill(pid_fryzjera, SIGTERM) == -1) {
                    perror("Błąd zwolnienia fryzjera");
                } else {
                    printf("Kierownik: Zwolniono fryzjera o PID %d.\n", pid_fryzjera);
                    aktywni_fryzjerzy--;
                    if (aktywni_fryzjerzy == 0) {
                        printf("Kierownik: Wszyscy fryzjerzy zostali zwolnieni. Kończę symulację.\n");
                        kill(0, SIGINT);
                        exit(0);
                    }
                }
            }
        } else {
            printf("Kierownik: Ewakuacja klientów - czyszczenie kolejki komunikatów.\n");
            wyczysc_kolejke(kolejka);
            operacja_semaforowa(semafor, 4, -1);
            printf("Kierownik: Dostęp do salonu zablokowany.\n");
            ewakuacja=1;
            sleep(5); //fryzjerzy, który aktualnie strzygą klientów powinni ich też wyrzucić z salonu
            ewakuacja=0;
            printf("Kierownik: Ewakuacja zakończona - odblokowanie dostępu.\n");
            operacja_semaforowa(semafor, 4, 1);
        }
    }
}