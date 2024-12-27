#include "funkcje.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

extern int semafor_poczekalnia;

void* klient(void* arg) {
    int klient_id = *(int*)arg;
    free(arg);

    while (1) {
        //printf("Klient %d przychodzi do salonu.\n", klient_id);

        // Sprawdzenie miejsca w poczekalni
        if (pobierz_wartosc_semafora(semafor_poczekalnia, 0) > 0) {
            semafor_p(semafor_poczekalnia, 0);
            //printf("Klient %d siada w poczekalni.\n", klient_id);
            
            sleep(rand() % 3 + 1);

            //printf("Klient %d jest obsługiwany przez fryzjera.\n", klient_id);
        } else {
            printf("Klient %d odchodzi, brak miejsca w poczekalni.\n", klient_id);
        }

        sleep(rand() % 5 + 1);
    }

    return NULL;
}