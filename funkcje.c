#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/msg.h>

// Tworzenie semafora
int utworz_semafor(key_t klucz, int liczba_semaforow) {
    int semafor_id = semget(klucz, liczba_semaforow, IPC_CREAT | 0666);
    if (semafor_id == -1) {
        perror("Błąd tworzenia semafora");
        exit(EXIT_FAILURE);
    }
    return semafor_id;
}

// Usuwanie semafora
void usun_semafor(int semafor_id) {
    if (semctl(semafor_id, 0, IPC_RMID) == -1) {
        perror("Błąd usuwania semafora");
        exit(EXIT_FAILURE);
    }
}

// Zwiększenie semafora
void semafor_v(int semafor_id, int numer_semafora) {
    struct sembuf bufor_sem;
    bufor_sem.sem_num = numer_semafora;
    bufor_sem.sem_op = 1;
    bufor_sem.sem_flg = 0;

    while (semop(semafor_id, &bufor_sem, 1) == -1) {
        if (errno == EINTR)
            continue;
        else {
            perror("Problem z otwarciem semafora");
            exit(EXIT_FAILURE);
        }
    }
}

// Zmniejszenie semafora
void semafor_p(int semafor_id, int numer_semafora) {
    struct sembuf bufor_sem;
    bufor_sem.sem_num = numer_semafora;
    bufor_sem.sem_op = -1;
    bufor_sem.sem_flg = 0;

    while (semop(semafor_id, &bufor_sem, 1) == -1) {
        if (errno == EINTR)
            continue;
        else {
            perror("Problem z zamknięciem semafora");
            exit(EXIT_FAILURE);
        }
    }
}

// Ustawienie wartości semafora
void inicjalizuj_semafor(int semafor_id, int numer_semafora, int wartosc) {
    if (semctl(semafor_id, numer_semafora, SETVAL, wartosc) == -1) {
        perror("Błąd inicjalizacji semafora");
        exit(EXIT_FAILURE);
    }
}

// Pobranie wartości semafora
int pobierz_wartosc_semafora(int semafor_id, int numer_semafora) {
    int wartosc = semctl(semafor_id, numer_semafora, GETVAL);
    if (wartosc == -1) {
        perror("Błąd pobrania wartości semafora");
        exit(EXIT_FAILURE);
    }
    return wartosc;
}
