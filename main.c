#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "structs.h"

//
static struct sembuf buf;
int serverId;
int *settings;
int semid;
struct Init initBuf;
struct Data data;
struct Build build;
struct Attack attack;
struct Alive alive;

void setSem(int key) {
    semid = semget(key + 1234, 2, IPC_CREAT | 0640);

    if (semctl(semid, 0, SETVAL, (int) 1) == -1) {
        perror("Nadanie wartosci semaforowi 0");
        exit(1);
    }
    if (semctl(semid, 1, SETVAL, (int) 1) == -1) {
        perror("Nadanie wartosci semaforowi 0");
        exit(1);
    }
}

void podnies(unsigned short int semnum) {
    buf.sem_num = semnum;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1) {
        perror("Podnoszenie semafora");
        exit(1);
    }

}

void opusc(unsigned short int semnum) {
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1) {
        perror("Opuszczenie semafora");
        exit(1);

    }
}

int zyje() {
    int lol;
    opusc(0);
    lol = settings[1];
    podnies(0);
    return lol;
}

int wyswietlac() {
    int lol;
    opusc(0);
    lol = settings[0];
    podnies(0);
    return lol;
}

void setWyswietlac(int lol) {
    opusc(0);
    settings[0] = lol;
    podnies(0);
}

void setZyje(int lol) {
    opusc(0);
    settings[1] = lol;
    podnies(0);
}


void pokaData(struct Data data, char *info[3]) {
    printf("Nacisnij 'a' aby atakowac, 'b' aby budowac, 'k' aby zakonczyc\n");
    printf("+-----------------+---------+\n");
    printf("| lekka piechota  | %6d |\n", data.light);
    printf("| ciezka piechota | %6d |\n", data.heavy);
    printf("| jazda           | %6d |\n", data.cavalry);
    printf("| robotnicy       | %6d |\n", data.workers);
    printf("+-----------------+---------+\n");
    printf("| surowce         | %6d |\n", data.resources);
    printf("| punkty          | %6d |\n", data.points);
    printf("+-----------------+---------+\n");
    if (data.info != NULL && strcmp(data.info, "") != 0) {
        info[2] = info[1];
        info[1] = info[0];
        info[0] = strdup(data.info);
    }
    int i = 0;
    for (; i < 3; i++) {
        if (strcmp(info[i], "") != 0)
            printf("%s\n", info[i]);
    }
}

void atak() {
    setWyswietlac(0);
    printf("Przeprowadzasz atak!\n");
    printf("Ile wyslac l. piechoty ");
    scanf("%d", &attack.light);
    printf("Ile wyslac c. piechoty ");
    scanf("%d", &attack.heavy);
    printf("Ile wyslac jazdy ");
    scanf("%d", &attack.cavalry);
    setWyswietlac(1);
    msgsnd(serverId, &attack, sizeof(attack) - sizeof(long), 0);
}


void buduj() {
    setWyswietlac(0);
    printf("Ile wybudowac l. piechoty ");
    scanf("%d", &build.light);
    printf("Ile wybudowac c. piechoty ");
    scanf("%d", &build.heavy);
    printf("Ile wybudowac jazdy ");
    scanf("%d", &build.cavalry);
    printf("Ile wybudowac robotnikow ");
    scanf("%d", &build.workers);
    setWyswietlac(1);
    msgsnd(serverId, &build, sizeof(build)  - sizeof(long), 0);
}

void setTypes() {
    initBuf.mtype = 1;
    data.mtype = 1;
    build.mtype = 2;
    attack.mtype = 3;
    alive.mtype = 4;
}

int main() {
    data.resources = 0;
    setTypes();

    int key = 15071410;

    int kolejkaInitId = msgget(key, IPC_CREAT | 0640);

    if (kolejkaInitId == -1) {
        perror("Utworzenie kolejki komunikatow");
        exit(1);
    }
    printf("czekam na serwer\n");
    while (msgrcv(kolejkaInitId, &initBuf, sizeof(initBuf.nextMsg), 1, 0) == -1) {
        printf("lolxdlolol");
    }
    printf("serwer wlaczony, id kolejki to %d\n", initBuf.nextMsg);
    setSem(initBuf.nextMsg);
    initBuf.mtype = 2;
    msgsnd(kolejkaInitId, &initBuf, sizeof(initBuf.nextMsg), 0);
    int p = shmget(initBuf.nextMsg + 20, 2 * sizeof(int), IPC_CREAT | 0640);
    settings = (int *) shmat(p, NULL, 0);
    printf("czekam na drugiego gracza\n");

    serverId = msgget(initBuf.nextMsg, IPC_CREAT | 0640);

    settings[0] = 1;
    settings[1] = 1;


    if (serverId == -1) {
        perror("Utworzenie kolejki komunikatow");
        exit(1);
    }

    if (fork() == 0) {
        char *infos[3];
        infos[0] = "  ";
        infos[1] = "  ";
        infos[2] = "  ";
        data.end = 0;
        int koniec = 0;
        while (!koniec) {
            msgrcv(serverId, &data, sizeof(data) - sizeof(long), 1, 0);
            settings = (int *) shmat(p, NULL, 0);
            if (wyswietlac()) {
                system("clear");
                pokaData(data, infos);
            }
            koniec = (int) data.end;
        }
    }
    else if (fork() == 0) {
        while (1) {
            msgsnd(serverId, &alive, sizeof(alive)  - sizeof(long), 0);
            sleep(2);
        }
    }
    else if (fork() == 0) {
        int recive;
        int howMany = 0;
        msgrcv(serverId, &alive, sizeof(alive) - sizeof(long), 5, 0);
        while (howMany < 3) {
            sleep(2);
            recive = msgrcv(serverId, &alive, sizeof(alive) - sizeof(long), 5, IPC_NOWAIT);
            if (recive < 0) {
                ++howMany;
                printf("serwer nie odpowiada, laczymy ponownie\n");
            }
            else {
                howMany = 0;
            }
            if (howMany > 2) {
                system("clear");
                printf("utracono polaczenie z serwerem\n");
                exit(0);
            }
        }
        exit(0);

    }
    else {
        while (zyje()) {
            char l;
            l = getchar();
            if (l == 'b' || l == 'B')
                buduj();
            if (l == 'a' || l == 'A')
                atak();
            if (l == 'k' || l == 'K')
                setZyje(0);
        }
    }
    shmctl(p, IPC_RMID, 0);
    kill(0, SIGKILL);
    exit(0);
}