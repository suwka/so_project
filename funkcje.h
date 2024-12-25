#ifndef FUNKCJE_H
#define FUNKCJE_H

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
int utworz_semafor(key_t klucz, int liczba_semaforow);

// Usuwanie semafora
void usun_semafor(int semafor_id);

// Zwiększenie semafora
void semafor_v(int semafor_id, int numer_semafora);

// Zmniejszenie semafora
void semafor_p(int semafor_id, int numer_semafora);

// Ustawienie wartości semafora
void inicjalizuj_semafor(int semafor_id, int numer_semafora, int wartosc);

// Pobranie wartości semafora
int pobierz_wartosc_semafora(int semafor_id, int numer_semafora);

#endif // FUNKCJE_H
