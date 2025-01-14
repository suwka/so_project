#ifndef FUNKCJE_H
#define FUNKCJE_H

#include <sys/types.h>
#include <signal.h>

#define FRYZJERZY 3
#define KLIENCI 20
#define POCZEKALNIA 10
#define FOTELE 3

#define LICZBA_NOMINALOW 3

#define KOSZT_USLUGI 30
#define KASA_STARTOWA_BANKNOTY {20, 20, 20}
#define KLIENT_STARTOWE_BANKNOTY {5, 5, 5}

#define CZAS_OTWARCIA 8
#define CZAS_ZAMKNIECIA 16
#define GODZINA 1


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

extern volatile sig_atomic_t ewakuacja;


//KOLORKI

#define KASA_COLOR "\033[1;91m"
#define KIEROWNIK_COLOR "\033[1;35m"
#define KLIENT_COLOR "\033[1;32m"
#define FRYZJER_COLOR "\033[1;34m"
#define ORANGE "\033[1;38;2;255;165;0m"
#define YELLOW "\033[1;33m"


#define PID_COLOR "\033[0m"
#define VALUE_COLOR "\033[0m"

// #define BOLD "\033[1m"
// #define ORANGE "\033[38;2;255;165;0m"


#endif
