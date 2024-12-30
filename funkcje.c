#include "funkcje.h"

int semafor, kolejka;

void inicjalizuj_zasoby() {
    key_t klucz = ftok("/tmp", 'S');

    semafor = semget(klucz, 3, IPC_CREAT | 0666);
    if (semafor == -1) {
        perror("Blad semafora");
        exit(EXIT_FAILURE);
    }

    kolejka = msgget(klucz, IPC_CREAT | 0666);
    if (kolejka == -1) {
        perror("Blad kolejki");
        exit(EXIT_FAILURE);
    }

    semctl(semafor, 0, SETVAL, POCZEKALNIA);
    semctl(semafor, 1, SETVAL, FOTELE);
    semctl(semafor, 2, SETVAL, 0);
}

void zwolnij_zasoby() {
    semctl(semafor, 0, IPC_RMID);
    msgctl(kolejka, IPC_RMID, NULL);
    printf("Zasoby zwolnione.\n");
    exit(0);
}

void operacja_semaforowa(int semid, int semnum, int operacja) {
    struct sembuf operacja_semaf = {semnum, operacja, 0};
    if (semop(semid, &operacja_semaf, 1) == -1) {
        perror("Blad operacji semaforowej");
        exit(EXIT_FAILURE);
    }
}
