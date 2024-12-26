int semafor_fotele; // Semafor reprezentujący dostępność foteli.

void* fryzjer(void* id) {
    int fryzjer_id = *(int*)id; // Pobranie identyfikatora fryzjera.
    free(id); // Zwolnienie pamięci przydzielonej dla ID.

    while (1) {
        printf("Fryzjer %d czeka na wolny fotel...\n", fryzjer_id);
        semafor_p(semafor_fotele, 0); // Zmniejszenie wartości semafora (sprawdzenie dostępności fotela).
        printf("Fryzjer %d zajmuje fotel i rozpoczyna pracę.\n", fryzjer_id);
        sleep(rand() % 3 + 1); // Symulacja czasu strzyżenia.
        printf("Fryzjer %d zwalnia fotel.\n", fryzjer_id);
        semafor_v(semafor_fotele, 0); // Zwiększenie wartości semafora (zwolnienie fotela).
        sleep(rand() % 2 + 1); // Symulacja czasu przerwy fryzjera.
    }
    return NULL;
}

int main() {
    srand(time(NULL)); // Inicjalizacja generatora liczb pseudolosowych.

    key_t klucz_fotele = ftok("./", 'F'); // Tworzenie klucza do identyfikacji semafora.
    semafor_fotele = utworz_semafor(klucz_fotele, 1); // Tworzenie semafora dla foteli.
    inicjalizuj_semafor(semafor_fotele, 0, MAX_FOTELI); // Inicjalizacja semafora wartością równą liczbie foteli.

    pthread_t fryzjerzy[MAX_FRYZJEROW]; // Tablica identyfikatorów wątków fryzjerów.

    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        int* id = malloc(sizeof(int)); // Dynamiczna alokacja pamięci dla identyfikatora fryzjera.
        if (id == NULL) { // Sprawdzenie, czy alokacja pamięci się powiodła.
            perror("Błąd alokacji pamięci dla ID fryzjera");
            exit(EXIT_FAILURE);
        }
        *id = i + 1; // Przypisanie identyfikatora fryzjera.
        if (pthread_create(&fryzjerzy[i], NULL, fryzjer, id) != 0) { // Tworzenie wątku dla fryzjera.
            perror("Błąd tworzenia wątku fryzjera");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_join(fryzjerzy[i], NULL); // Oczekiwanie na zakończenie wątków fryzjerów (teoretycznie nigdy się nie kończą).
    }

    usun_semafor(semafor_fotele); // Usunięcie semafora po zakończeniu programu.

    return 0;
}
