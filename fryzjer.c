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

*/


void proces_fryzjera() {
    prctl(PR_SET_NAME, "fryzjer", 0, 0, 0);
    while (1) {
        operacja_semaforowa(semafor, 2, -1);
        operacja_semaforowa(semafor, 1, -1);

        Wiadomosc wiad;
        if (msgrcv(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 1, 0) == -1 ) {
            perror("Błąd odbioru wiadomości");
            fprintf(stderr, "Kod błędu errno: %d, opis błędu: %s\n", errno, strerror(errno));
        };

        printf("Fryzjer %d strzyże klienta %d.\n", getpid(), wiad.id_klienta);
        sleep(wiad.czas);

        wiad.typ = wiad.id_klienta;
        if (msgsnd(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 0) == -1 ) {
            perror("Błąd wysyłania wiadomości");
            fprintf(stderr, "Kod błędu errno: %d, opis błędu: %s\n", errno, strerror(errno));
        };

        printf("Fryzjer %d zakończył strzyżenie klienta %d.\n", getpid(), wiad.id_klienta);
        operacja_semaforowa(semafor, 1, 1);

        printf("Fryzjer %d: Stan kasy:\n", getpid());
        int suma_kasy = oblicz_sume_banknotow(kasa->banknoty);

        for (int i = 0; i < LICZBA_NOMINALOW; i++) {
            printf("%d zl: %d banknotów\n", NOMINALY[i], kasa->banknoty[i]);
        }
        printf("Łączna suma w kasie: %d zł\n", suma_kasy);
    }
}
