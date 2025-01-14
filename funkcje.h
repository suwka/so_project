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
#define GODZINA 1   // Reprezentacja jednej godziny w systemie (np. 1 sekunda = 1 godzina)

// Struktura przechowująca liczbę banknotów w kasie
typedef struct {
    int banknoty[LICZBA_NOMINALOW];
} Kasa;

// Struktura reprezentująca klienta i jego banknoty
typedef struct {
    int banknoty[LICZBA_NOMINALOW];
} Klient;

// Struktura wiadomości używanej w kolejce komunikatów
typedef struct {
    long typ;         // Typ wiadomości (np. identyfikator fryzjera)
    int id_klienta;   // Identyfikator klienta
    int czas;         // Czas wykonania operacji
} Wiadomosc;

// Deklaracje zmiennych globalnych dostępnych w innych plikach
extern int semafor, kolejka, pamiec_kasy;
extern Kasa *kasa;
extern const int NOMINALY[LICZBA_NOMINALOW];

// Funkcje do inicjalizacji i zwalniania zasobów IPC
void inicjalizuj_zasoby();
void zwolnij_zasoby();

// Funkcje procesów w symulacji (kierownik, fryzjer, klient)
void proces_kierownika();
void proces_fryzjera();
void proces_klienta();

// Funkcja do wykonywania operacji na semaforach
void operacja_semaforowa(int semid, int semnum, int operacja);

// Funkcje obsługujące klientów
void inicjalizuj_klienta(Klient *klient);
int oblicz_sume_banknotow(const int *banknoty);
void dodaj_banknoty(int *cel, const int *zrodlo);
int zaplac(int *zrodlo, int kwota, int *cel);

// Funkcja do czyszczenia kolejki komunikatów
void wyczysc_kolejke(int kolejka);

// Zmienna globalna do obsługi stanu ewakuacji (np. zakończenia pracy salonu)
extern volatile sig_atomic_t ewakuacja;

// Definicje kolorów dla wypisywania informacji na ekranie
#define KASA_COLOR "\033[1;91m"      // Kolor czerwony dla informacji o kasie
#define KIEROWNIK_COLOR "\033[1;35m" // Kolor różowy dla informacji od kierownika
#define KLIENT_COLOR "\033[1;32m"    // Kolor zielony dla informacji o kliencie
#define FRYZJER_COLOR "\033[1;34m"   // Kolor niebieski dla informacji o fryzjerze
#define ORANGE "\033[1;38;2;255;165;0m" // Kolor pomarańczowy
#define YELLOW "\033[1;33m"          // Kolor żółty

// Resetowanie kolorów tekstu
#define PID_COLOR "\033[0m"          // Resetowanie koloru dla PID
#define VALUE_COLOR "\033[0m"        // Resetowanie koloru wartości

#endif
