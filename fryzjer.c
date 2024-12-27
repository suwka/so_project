#include "funkcje.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_FOTELE 5

extern int semafor_poczekalnia;
extern int semafor_fotele;

void* fryzjer(void* arg) {
    int fryzjer_id = *(int*)arg;
    free(arg);

    while (1) {
        printf("Fryzjer %d czeka na klienta.\n", fryzjer_id);

        // Sprawdza, czy jest klient w poczekalni
        semafor_p(semafor_poczekalnia, 0);

        printf("Fryzjer %d znalazl klienta i sadza go na fotelu.\n", fryzjer_id);

        // Zajmuje fotel
        semafor_p(semafor_fotele, 0);

        // Zajmuje klienta i wykonuje strzyżenie
        printf("Fryzjer %d strzyze klienta.\n", fryzjer_id);
        sleep(rand() % 3 + 1);

        printf("Fryzjer %d skonczyl strzyzenie klienta i zwalnia fotel.\n", fryzjer_id);

        // Zwolnienie fotela
        semafor_v(semafor_fotele, 0);

        // Sprawdzenie, czy są kolejni klienci w poczekalni
        // Fryzjer nie będzie czekał na puste fotelki, tylko kontynuuje pracę
        semafor_v(semafor_poczekalnia, 0); // Informuje, że fotel jest wolny
    }

    return NULL;
}