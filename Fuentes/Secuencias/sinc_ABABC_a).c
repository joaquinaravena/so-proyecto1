#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
// Declarar semáforos
sem_t semA, semB, semC;

void *printA(void *arg) {
    while (1) {
    sem_wait(&semA);
    printf("A");
    sem_post(&semB);
    }
}
void *printB(void *arg) {
    while (1) {
    sem_wait(&semB);
    printf("B");
    sem_post(&semC);
    }
}
void *printC(void *arg) {
    while (1) {
    sem_wait(&semC);
    sem_post(&semA);
    sem_wait(&semC);
    printf("C \n");
    sem_post(&semA);
    }
}


int main() {
pthread_t threadA, threadB, threadC;
// Inicializar semáforos
sem_init(&semA, 0, 1);
sem_init(&semB, 0, 0);
sem_init(&semC, 0, 0);

// Crear hilos
pthread_create(&threadA, NULL, printA, NULL);
pthread_create(&threadB, NULL, printB, NULL);
pthread_create(&threadC, NULL, printC, NULL);
// Esperar a que los hilos terminen (esto no sucederá)
pthread_join(threadA, NULL);
pthread_join(threadB, NULL);
pthread_join(threadC, NULL);
// Nunca llegamos aquí, ya que los hilos se ejecutan en bucle infinito
return 0;
}
