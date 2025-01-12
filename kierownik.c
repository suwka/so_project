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

// Licznik aktywnych fryzjerów
volatile int aktywni_fryzjerzy = FRYZJERZY;

void proces_kierownika() {
    srand(time(NULL));
    prctl(PR_SET_NAME, "kierownik", 0, 0, 0);
    while (1) {
        sleep(rand() % 10 + 5); // 5-15 sekund

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

                    // Sprawdź, czy wszyscy fryzjerzy zostali zwolnieni
                    if (aktywni_fryzjerzy == 0) {
                        printf("Kierownik: Wszyscy fryzjerzy zostali zwolnieni. Kończę symulację.\n");
                        kill(0, SIGINT); // Wysyła sygnał SIGINT do wszystkich procesów w grupie
                        exit(0);
                    }
                }
            }
        } else {
            printf("Kierownik: Ewakuacja klientów!\n");

            // Zablokowanie wejścia do salonu
            semctl(semafor, 4, SETVAL, 0);

            ewakuacja = 1; // Ustawienie globalnej flagi ewakuacji
            for (int i = 0; i < KLIENCI; i++) {
                pid_t pid_klienta = getpid() + FRYZJERZY + i + 1;
                kill(pid_klienta, SIGUSR1); // Wysłanie sygnału ewakuacji
            }

            // Czekanie na zakończenie ewakuacji
            sleep(5); // Czas na ewakuację klientów
            printf("Kierownik: Ewakuacja zakończona.\n");

            // Odblokowanie wejścia do salonu
            semctl(semafor, 4, SETVAL, 1);
            ewakuacja = 0; // Reset flagi ewakuacji
        }
    }
}