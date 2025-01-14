#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include "funkcje.h"
#include <sys/prctl.h>
#include <string.h>
#include <errno.h>


/*
        to do:
            godziny otwarcia zamkniecia salonu (dodac czas symulacji)
            w przypadku zwolnienia wszystkich fryzjerow koniec symulacji
            synchronizacja zamkniecia salonu na czas ewakuacji

*/


void proces_fryzjera() {
    prctl(PR_SET_NAME, "fryzjer", 0, 0, 0);
    while (1) {
        // Oczekiwanie na klienta
        operacja_semaforowa(semafor, 2, -1);
        operacja_semaforowa(semafor, 1, -1);

        // Odbiór wiadomości o kliencie
        Wiadomosc wiad;
        if (msgrcv(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 1, 0) == -1) {
            perror("Błąd odbioru wiadomości");
        };

        printf("Fryzjer %d strzyże klienta %d.\n", getpid(), wiad.id_klienta);

        int czas_strzyzenia = wiad.czas;
        for (int i = 0; i < czas_strzyzenia; i++) {
            if (ewakuacja) {
                printf("Fryzjer %d: Przerwano strzyżenie z powodu ewakuacji. Klient %d opuszcza salon.\n", 
                       getpid(), wiad.id_klienta);
                break;
            }
            sleep(1); // Symulacja upływu czasu strzyżenia
        }

        // Zakończenie lub przerwanie usługi
        if (!ewakuacja) {
            printf("Fryzjer %d zakończył strzyżenie klienta %d.\n", getpid(), wiad.id_klienta);
            wiad.typ = wiad.id_klienta;
            if (msgsnd(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 0) == -1) {
                perror("Błąd wysyłania wiadomości");
            }
        }

        operacja_semaforowa(semafor, 1, 1); // Zwolnienie fotela
    }
}
