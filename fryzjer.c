#include "funkcje.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_FRYZJEROW 5
#define MAX_FOTELI 3

int semafor_fotele; // Semafor do zarzadzania fotelami

void* fryzjer(void* id) {
    int fryzjer_id = *(int*)id;
    free(id);

    while (1) {
        printf("Fryzjer %d czeka na wolny fotel...\n", fryzjer_id);

        // Sprawdzenie dostępności fotela
        semafor_p(semafor_fotele, 0);

        printf("Fryzjer %d zajmuje fotel i rozpoczyna pracę.\n", fryzjer_id);
        sleep(rand() % 3 + 1); // Symulacja strzyżenia

        printf("Fryzjer %d zwalnia fotel.\n", fryzjer_id);
        semafor_v(semafor_fotele, 0);

        sleep(rand() % 2 + 1); // Czas na odpoczynek fryzjera
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    // Tworzenie semafora dla foteli
    key_t klucz_fotele = ftok("./", 'F');
    semafor_fotele = utworz_semafor(klucz_fotele, 1);
    inicjalizuj_semafor(semafor_fotele, 0, MAX_FOTELI);

    pthread_t fryzjerzy[MAX_FRYZJEROW];

    for (int i = 0; i < MAX_FRYZJEROW; i++) {
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

    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }

    // Usuwanie semafora
    usun_semafor(semafor_fotele);

    return 0;
}
