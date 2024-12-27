#include "funkcje.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_POCZEKALNIA 10

int semafor_poczekalnia; // Semafor do zarzadzania poczekalnia

void* klient(void* arg) {
    int klient_id = *(int*)arg;
    free(arg);

    printf("Klient %d przychodzi do salonu.\n", klient_id);

    if (pobierz_wartosc_semafora(semafor_poczekalnia, 0) > 0) {
        // Klient zajmuje miejsce w poczekalni
        semafor_p(semafor_poczekalnia, 0);
        printf("Klient %d siada w poczekalni.\n", klient_id);

        // Czeka na obsługę przez fryzjera
        sleep(rand() % 3 + 1);

        printf("Klient %d jest obsługiwany przez fryzjera.\n", klient_id);

        // Po obsłudze zwalnia miejsce w poczekalni
        semafor_v(semafor_poczekalnia, 0);
        printf("Klient %d opuszcza salon.\n", klient_id);
    } else {
        // Brak miejsc w poczekalni
        printf("Klient %d odchodzi z powodu braku miejsc w poczekalni.\n", klient_id);
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    // Tworzenie semafora dla poczekalni
    key_t klucz_poczekalnia = ftok("./", 'P');
    semafor_poczekalnia = utworz_semafor(klucz_poczekalnia, 1);
    inicjalizuj_semafor(semafor_poczekalnia, 0, MAX_POCZEKALNIA);

    pthread_t klienci[20];

    for (int i = 0; i < 20; i++) {
        sleep(rand() % 2 + 1); // Klienci przychodzą w losowych momentach

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
    }

    for (int i = 0; i < 20; i++) {
        pthread_join(klienci[i], NULL);
    }

    // Usuwanie semafora
    usun_semafor(semafor_poczekalnia);

    return 0;
}