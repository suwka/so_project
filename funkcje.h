#ifndef FUNKCJE_H
#define FUNKCJE_H

#define FRYZJERZY 3
#define KLIENCI 20
#define POCZEKALNIA 10
#define FOTELE 3

#define LICZBA_NOMINALOW 3

#define KOSZT_USLUGI 30
#define KASA_STARTOWA_BANKNOTY {20, 20, 20}
#define KLIENT_STARTOWE_BANKNOTY {5, 5, 5}

#include <sys/types.h>
#include <signal.h>

typedef struct {
    int banknoty[LICZBA_NOMINALOW];
} Kasa;

typedef struct {
    int banknoty[LICZBA_NOMINALOW];
} Klient;

typedef struct {
    long typ;
    int id_klienta;
    int czas;
} Wiadomosc;

extern int semafor, kolejka, pamiec_kasy;
extern Kasa *kasa;
extern const int NOMINALY[LICZBA_NOMINALOW];

void inicjalizuj_zasoby();
void zwolnij_zasoby();
void proces_kierownika();
void proces_fryzjera();
void proces_klienta();
void operacja_semaforowa(int semid, int semnum, int operacja);

void inicjalizuj_klienta(Klient *klient);
int oblicz_sume_banknotow(const int *banknoty);
void dodaj_banknoty(int *cel, const int *zrodlo);
int zaplac(int *zrodlo, int kwota, int *cel);

void wyczysc_kolejke(int kolejka);

extern volatile sig_atomic_t ewakuacja; // Deklaracja zmiennej globalnej



#endif
