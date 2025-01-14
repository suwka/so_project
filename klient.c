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

volatile sig_atomic_t ewakuacja = 0;    //flaga informujaca o ewakuacji strzyzonego (0 brak ewakuacji, 1 ewakuacja)

void obsluz_sygnal(int sig) {
    printf(KLIENT_COLOR "Klient " PID_COLOR "%d" KLIENT_COLOR ": Otrzymałem sygnał ewakuacji.\n" PID_COLOR, getpid());
    ewakuacja = 1;      //ewakuacja
}

void proces_klienta() {
    signal(SIGUSR1, obsluz_sygnal);     //obsluga sygnalu ewakuacji
    Klient klient;
    inicjalizuj_klienta(&klient);

    srand(getpid());
    prctl(PR_SET_NAME, "klient", 0, 0, 0);

    while (1) {
        if (ewakuacja) {
            operacja_semaforowa(semafor, 0, 1);     //zwalnia miejsce w poczekalni
            operacja_semaforowa(semafor, 1, 1);     //zwalnia fotel fryzjerski
            ewakuacja = 0;
            sleep(rand() % 5 + 1);
            continue;
        }

        sleep(rand() % 3 + 1);

        //proba wejscia do salonu, jezeli ewakuacja to sie nie da
        int czy_ewakuacja = semctl(semafor, 4, GETVAL);
        if (czy_ewakuacja == 0) {
            sleep(rand() % 5 + 1);
            continue;
        }

        printf(KLIENT_COLOR "Klient " PID_COLOR "%d" KLIENT_COLOR " przychodzi do salonu.\n" PID_COLOR, getpid());
        if (semop(semafor, (struct sembuf[]){{0, -1, IPC_NOWAIT}}, 1) == -1) {
            printf(KLIENT_COLOR "Klient " PID_COLOR "%d" KLIENT_COLOR " opuszcza salon, bo jest pełno.\n" PID_COLOR, getpid());
            sleep(rand() % 5 + 1);
            int losowy_nominal = rand() % LICZBA_NOMINALOW;
            klient.banknoty[losowy_nominal]++;
            printf(KLIENT_COLOR "Klient " PID_COLOR "%d" KLIENT_COLOR " zarabia banknot " VALUE_COLOR "%d" KLIENT_COLOR " zł.\n" PID_COLOR, getpid(), NOMINALY[losowy_nominal]);
            continue;
        }

        printf(KLIENT_COLOR "Klient " PID_COLOR "%d" KLIENT_COLOR " czeka na fotel.\n" PID_COLOR, getpid());
        operacja_semaforowa(semafor, 2, 1); //informuje fryzjera o gotowosci do strzyzenia

        //czy stac na strzyzenie
        if (oblicz_sume_banknotow(klient.banknoty) < KOSZT_USLUGI) {
            printf(KLIENT_COLOR "Klient " PID_COLOR "%d" KLIENT_COLOR " nie ma wystarczająco pieniędzy, opuszcza salon.\n" PID_COLOR, getpid());
            operacja_semaforowa(semafor, 0, 1);     //zwalnia miejsce w poczekalni
            operacja_semaforowa(semafor, 2, -1);    //wycofuje gotowosc fryzjera
            continue;
        }

        if (zaplac(klient.banknoty, KOSZT_USLUGI, kasa->banknoty)) {
            printf(KLIENT_COLOR "Klient " PID_COLOR "%d" KLIENT_COLOR " płaci " VALUE_COLOR "%d" KLIENT_COLOR " zł za usługę.\n" PID_COLOR, getpid(), KOSZT_USLUGI);
        } else {
            printf(KLIENT_COLOR "Klient " PID_COLOR "%d" KLIENT_COLOR " ma problem z płatnością.\n" PID_COLOR, getpid());
            operacja_semaforowa(semafor, 0, 1);
            operacja_semaforowa(semafor, 2, -1);
            continue;
        }

        //synchronizacja czasu strzyzenia, klient wysyla do fryzjera czas obslugi
        Wiadomosc wiad = {1, getpid(), rand() % 3 + 1};
        msgsnd(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 0);

        //odbieranie wiadomosci od fryzjera ze strzyzenie sie zakonczylo
        msgrcv(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), getpid(), 0);

        operacja_semaforowa(semafor, 1, 1); //zwolnienie fotela
        operacja_semaforowa(semafor, 0, 1); //zwolnienie miejsca w poczekalni

        sleep(rand() % 30 + 1);
    }
}
