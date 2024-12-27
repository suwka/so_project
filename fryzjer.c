#include "funkcje.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


extern int semafor_poczekalnia;
extern int semafor_fotele;

void* fryzjer(void* arg) {
    int fryzjer_id = *(int*)arg;
    free(arg);

    while (1) {
        printf("Fryzjer %d czeka na klienta.\n", fryzjer_id);

        semafor_p(semafor_poczekalnia, 0);

        //IDENTYFIKACJA FOTELI (nw czy poprawnie ale dziala - do przepatrzenia)
        static int fotel_id = 0;
        int aktualny_fotel = fotel_id;
        fotel_id = ((fotel_id + 1) % MAX_FOTELE)+1;

        printf("Fryzjer %d znalazł klienta i sadza go na fotelu %d.\n", fryzjer_id, aktualny_fotel);

        semafor_p(semafor_fotele, 0);

        printf("Fryzjer %d strzyże klienta na fotelu %d.\n", fryzjer_id, aktualny_fotel);
        sleep(rand() % 3 + 1);

        printf("Fryzjer %d skończył strzyżenie klienta na fotelu %d i zwalnia fotel.\n", fryzjer_id, aktualny_fotel);

        semafor_v(semafor_fotele, 0);
        semafor_v(semafor_poczekalnia, 0);
    }

    return NULL;
}


/*
    pierwszy semafor sprawdza czy kliencik czeka w poczekalni
    drugi semaforek rezerwuje fotel
    na koncu zwalnianie semforów
*/