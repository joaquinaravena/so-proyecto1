#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define SIZE 1

// Declarar pipes
int pipeA[2], pipeB[2], pipeC[2], pipeD[2];
char buffer[SIZE];

void printA(){
    // Pipes que no se leen
    // cierra todos menos los que hace wait
    close(pipeB[0]);
    close(pipeC[0]);
    close(pipeD[0]);
    // Pipes que no se escriben
    //cierra todos menos los que manda signal
    close(pipeA[1]);
    close(pipeC[1]);
    close(pipeD[1]);
    while (1){
        read(pipeA[0], buffer, SIZE);
        printf("A");
        fflush(NULL); // limpiar buffer y escribir data
        write(pipeB[1], buffer, SIZE);
    }
}

void printB(){
    // Pipes que no se leen
    // cierra todos menos los que hace wait
    close(pipeA[0]);
    close(pipeC[0]);
    close(pipeD[0]);

    // Pipes que no se escriben
    //cierra todos menos los que manda signal
    close(pipeA[1]);
    close(pipeB[1]);
    close(pipeD[1]);
    while (1){
        read(pipeB[0], buffer, SIZE);
        printf("B");
        fflush(NULL); // limpiar buffer y escribir data
        write(pipeC[1], buffer, SIZE);
    }
}

void printC(){
    // Pipes que no se leen
    // cierra todos menos los que hace wait
    close(pipeA[0]);
    close(pipeB[0]);
    close(pipeD[0]);

    // Pipes que no se escriben
    //cierra todos menos los que manda signal
    close(pipeB[1]);
    close(pipeC[1]);
    while (1){
        read(pipeC[0], buffer, SIZE);
        fflush(NULL);
        write(pipeA[1], buffer, SIZE);
        read(pipeC[0], buffer, SIZE);
        printf("C");
        fflush(NULL);
        write(pipeA[1], buffer, SIZE);
        read(pipeC[0], buffer, SIZE);
        printf("C");
        fflush(NULL);
        write(pipeD[1], buffer, SIZE);
    }
}

void printD(){
    // Pipes que no se leen
    // cierra todos menos los que hace wait
    close(pipeA[0]);
    close(pipeB[0]);
    close(pipeC[0]);
    // Pipes que no se escriben
    //cierra todos menos los que manda signal
    close(pipeB[1]);
    close(pipeC[1]);
    close(pipeD[1]);
    while (1){
        read(pipeD[0], buffer, SIZE);
        printf("D \n");
        fflush(NULL); // limpiar buffer y escribir data
        write(pipeA[1], buffer, SIZE);
    }
}

int main() {
    int *arregloPipe[4] = {pipeA, pipeB, pipeC, pipeD};
    pid_t childs [4];
    for (int i = 0; i < 4; i++){
        if (pipe(arregloPipe[i]) == -1){
            printf("ERROR AL CREAR PIPE.");
            return -1;
        }
    }

    write(pipeA[1], buffer, SIZE);

    for (int i = 0; i < 4; i++){
        childs[i] = fork();
        if (childs[i] == 0){
            switch (i){
                case 0: printA(); break;
                case 1: printB(); break;
                case 2: printC(); break;
                case 3: printD(); break;
            }
        } else{
            if (childs[i] < 0){
                printf("ERROR AL CREAR HIJO");
            return -1;
            }
        }
    }

    // Esperar por los hijos
    for (int c = 0; c < 4; c++){
        if (childs[c] > 0 )
            wait(NULL);
        exit(0);
    }
}
