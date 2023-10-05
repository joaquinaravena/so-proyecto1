#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define NUM_HORAS 12
#define PRIMERA_HORA 9
#define LIBRE -1
#define NUM_HILOS 25

sem_t mutex;
int tabla_reservas[NUM_HORAS];

void reservar(int index){
    srand(time(NULL)+index);
    int hora_res = rand() %12;

    sem_wait(&mutex);
        if(tabla_reservas[hora_res] == LIBRE) {
            tabla_reservas[hora_res] = index;
            printf("El alumno %i reservo en la hora %i \n", index, hora_res + PRIMERA_HORA);
        }else
            printf("El alumno %i no pudo reservar en la hora %i \n", index, hora_res + PRIMERA_HORA);
    sem_post(&mutex);
}

void cancelar(int index) {
    int hora_res = LIBRE;
    for (int i = 1; i < NUM_HORAS && hora_res == LIBRE; i++) {
        if(tabla_reservas[i] == index)
            hora_res = i;
    }
    if (hora_res != LIBRE) {
        sem_wait(&mutex);
            tabla_reservas[hora_res] = LIBRE;
            printf("Turno %i cancelado por el alumno %i\n", hora_res + PRIMERA_HORA ,index);
        sem_post(&mutex);
    }else
        printf("El alumno %i no tiene turnos para cancelar\n", index);
}

void consultar(int index){
    srand(time(NULL)+index);
    int hora_res = rand() %12;
    if(tabla_reservas[hora_res] == LIBRE) {
        printf("Consulta el alumno %i la hora %i: libre\n", index, hora_res + PRIMERA_HORA);
    }else
        printf("Consulta el alumno %i la hora %i: reservada\n", index, hora_res + PRIMERA_HORA);
}

void decidir(int index) {
    srand(time(NULL)+index);
    int rnd = rand() % 100;

    if (rnd < 50)
        reservar(index);
    else if (rnd< 75)
        cancelar(index);
    else
        consultar(index);
}

void * accion_alumnos(void* arg) {
    int index = (int)arg;
    for (int i = 0; i < 4; i++){
        decidir(index);
        sleep(5);
    }
    return 0;
}

int main(){
    pthread_t hilos[NUM_HILOS];
    if (sem_init(&mutex, 0, 1) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    //Inicializo la tabla
    for(int i = 0; i < NUM_HORAS; i++){
        tabla_reservas[i] = -1;
    }
    //Creacion de hilos
    for (int i = 1; i <= NUM_HILOS; i++){
        pthread_create(&hilos[i-1], NULL, accion_alumnos,(void *)i);
        sleep(1);
    }
    //Espero que los hilos terminen
    for (int  i= 0; i < NUM_HILOS; i++){
        pthread_join(hilos[i], NULL);
    }

    return 0;
}
