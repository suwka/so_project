#include "funkcje.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_FOTELE 5 // Liczba foteli w salonie
#define MAX_POCZEKALNIA 10 // Liczba miejsc w poczekalni

int semafor_poczekalnia; // Semafor do zarzadzania poczekalnia
int semafor_fotele;     // Semafor do zarzadzania fotelami

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

        // Symulacja strzyzenia
        printf("Fryzjer %d strzyze klienta.\n", fryzjer_id);
        sleep(rand() % 3 + 1);

        printf("Fryzjer %d skonczyl strzyzenie klienta i zwalnia fotel.\n", fryzjer_id);

        // Zwolnienie fotela
        semafor_v(semafor_fotele, 0);
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    // Tworzenie semaforow dla poczekalni i foteli
    key_t klucz_poczekalnia = ftok("./", 'P');
    key_t klucz_fotele = ftok("./", 'F');

    semafor_poczekalnia = utworz_semafor(klucz_poczekalnia, 1);
    semafor_fotele = utworz_semafor(klucz_fotele, 1);

    inicjalizuj_semafor(semafor_poczekalnia, 0, MAX_POCZEKALNIA); // Ustawienie miejsc w poczekalni
    inicjalizuj_semafor(semafor_fotele, 0, MAX_FOTELE);

    pthread_t fryzjerzy[3];

    for (int i = 0; i < 3; i++) {
        int* id = malloc(sizeof(int));
        if (id == NULL) {
            perror("Błąd alokacji pamięci dla ID fryzjera");
            exit(EXIT_FAILURE);
        }
        *id = i + 1;

        if (pthread_create(&fryzjerzy[i], NULL, fryzjer, id) != 0) {
            perror("Błąd tworzenia wątku fryzjera");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }

    // Usuwanie semaforow
    usun_semafor(semafor_poczekalnia);
    usun_semafor(semafor_fotele);

    return 0;
}
