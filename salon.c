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

int semafor, kolejka;

void inicjalizuj_zasoby();
void zwolnij_zasoby();
void proces_klienta();
void proces_fryzjera();
void operacja_semaforowa(int semid, int semnum, int operacja);

int main() {
    signal(SIGINT, zwolnij_zasoby);
    inicjalizuj_zasoby();

    for (int i = 0; i < FRYZJERZY; i++) {
        if (fork() == 0) {
            proces_fryzjera();
            exit(0);
        }
    }

    for (int i = 0; i < KLIENCI; i++) {
        if (fork() == 0) {
            proces_klienta();
            exit(0);
        }
    }

    for (int i = 0; i < FRYZJERZY + KLIENCI; i++) {
        wait(NULL);
    }

    zwolnij_zasoby();
    return 0;
}

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

void proces_klienta() {
    srand(getpid());
    while (1) {
        sleep(rand() % 3 + 1);

        printf("Klient %d przychodzi do salonu.\n", getpid());
        if (semop(semafor, (struct sembuf[]){ {0, -1, IPC_NOWAIT} }, 1) == -1) {
            printf("Klient %d opuszcza salon.\n", getpid());
            sleep(rand() % 5 + 1); // Klient wraca po pewnym czasie
            continue;
        }

        printf("Klient %d czeka na fotel.\n", getpid());
        operacja_semaforowa(semafor, 2, 1);

        Wiadomosc wiad = {1, getpid(), rand() % 3 + 1};
        msgsnd(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 0);

        msgrcv(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), getpid(), 0);
        printf("Klient %d jest strzyzony.\n", getpid());
        sleep(wiad.czas);

        printf("Klient %d wychodzi.\n", getpid());
        operacja_semaforowa(semafor, 1, 1);
        operacja_semaforowa(semafor, 0, 1);

        sleep(rand() % 30 + 1); // Klient wraca po pewnym czasie
    }
}


void proces_fryzjera() {
    while (1) {
        operacja_semaforowa(semafor, 2, -1);

        operacja_semaforowa(semafor, 1, -1);
        Wiadomosc wiad;
        msgrcv(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 1, 0);

        printf("Fryzjer %d strzyze klienta %d.\n", getpid(), wiad.id_klienta);
        sleep(wiad.czas);

        wiad.yp = wiad.id_klienta;
        msgsnd(kolejka, &wiad, sizeof(Wiadomosc) - sizeof(long), 0);

        printf("Fryzjer %d zakonczyl strzyzenie klienta %d.\n", getpid(), wiad.id_klienta);
        operacja_semaforowa(semafor, 1, 1);
    }
}

void operacja_semaforowa(int semid, int semnum, int operacja) {
    struct sembuf operacja_semaf = {semnum, operacja, 0};
    if (semop(semid, &operacja_semaf, 1) == -1) {
        perror("Blad operacji semaforowej");
        exit(EXIT_FAILURE);
    }
}
