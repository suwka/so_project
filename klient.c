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
#include <string.h>
#include <sys/prctl.h>


void proces_klienta() {
    Klient klient;
    inicjalizuj_klienta(&klient);

    srand(getpid());
    prctl(PR_SET_NAME, "klient", 0, 0, 0);
    while (1) {
        sleep(rand() % 3 + 1);

        printf("Klient %d przychodzi do salonu.\n", getpid());
        if (semop(semafor, (struct sembuf[]){{0, -1, IPC_NOWAIT}}, 1) == -1) {
            printf("Klient %d opuszcza salon.\n", getpid());
            sleep(rand() % 5 + 1);
            int losowy_nominal = rand() % LICZBA_NOMINALOW;
            klient.banknoty[losowy_nominal]++;
            printf("Klient %d zarabia banknot %d zl.\n", getpid(), NOMINALY[losowy_nominal]);
            continue;
        }

        printf("Klient %d czeka na fotel.\n", getpid());
        operacja_semaforowa(semafor, 2, 1);

        if (oblicz_sume_banknotow(klient.banknoty) < KOSZT_USLUGI) {
            printf("Klient %d nie ma wystarczająco pieniędzy, opuszcza salon.\n", getpid());
            operacja_semaforowa(semafor, 0, 1);
            operacja_semaforowa(semafor, 2, -1);
            continue;
        }

        if (zaplac(klient.banknoty, KOSZT_USLUGI, kasa->banknoty)) {
            printf("Klient %d płaci %d zl za usługę.\n", getpid(), KOSZT_USLUGI);
        } else {
            printf("Klient %d ma problem z płatnością.\n", getpid());
            operacja_semaforowa(semafor, 0, 1);
            operacja_semaforowa(semafor, 2, -1);
            continue;
        }

        Wiadomosc wiad = {1, getpid(), rand() % 3 + 1};
        msgsnd(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 0);

        msgrcv(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), getpid(), 0);

        printf("Klient %d wychodzi.\n", getpid());
        operacja_semaforowa(semafor, 1, 1);
        operacja_semaforowa(semafor, 0, 1);

        sleep(rand() % 30 + 1);
    }
}
