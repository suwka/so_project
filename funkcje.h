#ifndef FUNKCJE_H
#define FUNKCJE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define FRYZJERZY 5
#define KLIENCI 20
#define FOTELE 3
#define POCZEKALNIA 10

typedef struct {
    long typ;
    int id_klienta;
    int czas;
} Wiadomosc;

extern int semafor, kolejka;

void inicjalizuj_zasoby();
void zwolnij_zasoby();
void proces_klienta();
void proces_fryzjera();
void operacja_semaforowa(int semid, int semnum, int operacja);

#endif