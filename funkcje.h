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

#define LICZBA_FRYZJEROW 5
#define LICZBA_KLIENTOW 15
#define MAX_POCZEKALNIA 10
#define MAX_FOTELE 3

int utworz_semafor(key_t klucz, int liczba_semaforow);
void usun_semafor(int semafor_id);
void semafor_v(int semafor_id, int numer_semafora);
void semafor_p(int semafor_id, int numer_semafora);
void inicjalizuj_semafor(int semafor_id, int numer_semafora, int wartosc);
int pobierz_wartosc_semafora(int semafor_id, int numer_semafora);

#endif