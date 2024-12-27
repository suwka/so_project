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

        // Sprawdza, czy jest klient w poczekalni
        semafor_p(semafor_poczekalnia, 0);

        // Przydziela fotel fryzjerowi
        static int fotel_id = 0; // Numeracja foteli (statyczna zmienna, współdzielona)
        int aktualny_fotel = fotel_id; // Aktualny fotel dla tego fryzjera
        fotel_id = ((fotel_id + 1) % MAX_FOTELE)+1; // Cykl przez fotele

        printf("Fryzjer %d znalazł klienta i sadza go na fotelu %d.\n", fryzjer_id, aktualny_fotel);

        // Zajmuje fotel
        semafor_p(semafor_fotele, 0);

        // Wykonuje strzyżenie
        printf("Fryzjer %d strzyże klienta na fotelu %d.\n", fryzjer_id, aktualny_fotel);
        sleep(rand() % 3 + 1);

        printf("Fryzjer %d skończył strzyżenie klienta na fotelu %d i zwalnia fotel.\n", fryzjer_id, aktualny_fotel);

        // Zwolnienie fotela
        semafor_v(semafor_fotele, 0);

        // Informuje, że fotel jest wolny
        semafor_v(semafor_poczekalnia, 0);
    }

    return NULL;
}
