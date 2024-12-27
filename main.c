#include "funkcje.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int semafor_poczekalnia;
int semafor_fotele;

void* fryzjer(void* arg);
void* klient(void* arg);

int main() {
    srand(time(NULL));

    key_t klucz_poczekalnia = ftok("./", 'P');
    key_t klucz_fotele = ftok("./", 'F');

    semafor_poczekalnia = utworz_semafor(klucz_poczekalnia, 1);
    semafor_fotele = utworz_semafor(klucz_fotele, 1);

    inicjalizuj_semafor(semafor_poczekalnia, 0, MAX_POCZEKALNIA);
    inicjalizuj_semafor(semafor_fotele, 0, MAX_FOTELE);

    pthread_t fryzjerzy[LICZBA_FRYZJEROW];
    pthread_t klienci[LICZBA_KLIENTOW];

    // Tworzenie wątków fryzjerów
    for (int i = 0; i < LICZBA_FRYZJEROW; i++) {
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

    // Tworzenie wątków klientów
    for (int i = 0; i < LICZBA_KLIENTOW; i++) {
        int* id = malloc(sizeof(int));
        if (id == NULL) {
            perror("Błąd alokacji pamięci dla ID klienta");
            exit(EXIT_FAILURE);
        }
        *id = i + 1;

        if (pthread_create(&klienci[i], NULL, klient, id) != 0) {
            perror("Błąd tworzenia wątku klienta");
            exit(EXIT_FAILURE);
        }
        usleep(rand() % 5000000);
    }

    // Oczekiwanie na zakończenie pracy fryzjerów
    for (int i = 0; i < LICZBA_FRYZJEROW; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }

    usun_semafor(semafor_poczekalnia);
    usun_semafor(semafor_fotele);

    return 0;
}