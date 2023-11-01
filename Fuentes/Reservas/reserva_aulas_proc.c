#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>

#define NUM_HORAS 12
#define PRIMERA_HORA 9
#define LIBRE -1
#define NUM_PROCESOS 25
#define TIEMPO_ENTRE_ACCIONES 5

struct shmemory{
    int tabla_reservas[NUM_HORAS]; //Los horarios son de 9:00 a 21:00
    sem_t mutex_tabla;
    sem_t consultantes;
    sem_t acceso_consultantes;
};

void reservar(int index, struct shmemory* shmem) {
    srand(time(NULL) + index);
    int hora_res = rand() % 12;
    sem_wait(&shmem->mutex_tabla);
    if (shmem->tabla_reservas[hora_res] == LIBRE) {
        shmem->tabla_reservas[hora_res] = index;
        printf("El alumno %i reservo en la hora %i \n", index, hora_res + PRIMERA_HORA);
    } else
        printf("El alumno %i no pudo reservar en la hora %i \n", index, hora_res + PRIMERA_HORA);
    sem_post(&shmem->mutex_tabla);
}

void cancelar(int index, struct shmemory* shmem) {
    int hora_res = LIBRE;
    sem_wait(&shmem->mutex_tabla);
    for (int i = 1; i < NUM_HORAS && hora_res == LIBRE; i++) {
        if (shmem->tabla_reservas[i] == index)
            hora_res = i;
    }
    if (hora_res != LIBRE) {
        shmem->tabla_reservas[hora_res] = LIBRE;
        printf("Turno %i cancelado por el alumno %i\n", hora_res + PRIMERA_HORA, index);
    } else
        printf("El alumno %i no tiene turnos para cancelar\n", index);
    sem_post(&shmem->mutex_tabla);
}

void consultar(int index, struct shmemory* shmem) {
    srand(time(NULL) + index);
    int hora_res = rand() % 12;
    int tiempo_consulta = (rand() + hora_res) % 4; // máximo 3 segundos de consulta

    //Protocolo de entrada
    sem_wait(&shmem->acceso_consultantes);
    if (sem_trywait(&shmem->consultantes) == -1) { // no habia ningún consultante activo
        sem_wait(&shmem->mutex_tabla);
    } else {
        sem_post(&shmem->consultantes);
    }
    sem_post(&shmem->acceso_consultantes);
    sem_post(&shmem->consultantes);

    // Acceso a tabla
    if (shmem->tabla_reservas[hora_res] == LIBRE) {
        printf("Consulta el alumno %i la hora %i: libre\n", index, hora_res + PRIMERA_HORA);
    } else
        printf("Consulta el alumno %i la hora %i: reservada\n", index, hora_res + PRIMERA_HORA);
    sleep(tiempo_consulta);
    printf("El alumno %i termino de consultar\n", index);

    //Protocolo de salida
    sem_wait(&shmem->consultantes);
    sem_wait(&shmem->acceso_consultantes);
    if(sem_trywait(&shmem->consultantes) == -1){ //el consultante era el último
        sem_post(&shmem->mutex_tabla);
    }else{
        sem_post(&shmem->consultantes);
    }
    sem_post(&shmem->acceso_consultantes);
}

void decidir(int index) {
    //Obtener memoria compartida
    key_t key;
    key = ftok("/tmp", 'B');
    struct shmemory* shmem;
    int size = sizeof(struct shmemory);
    int shmid = shmget(key, size, IPC_CREAT | 0666);
    shmem = (struct shmemory *) shmat(shmid, 0, 0);

    srand(time(NULL) + index);
    int rnd = rand() % 100;

    if (rnd < 50)
        reservar(index, shmem);
    else if (rnd < 75)
        cancelar(index, shmem);
    else
        consultar(index, shmem);
}

void accion_alumnos(int index) {
    for (int i = 0; i < 4; i++){
        decidir(index);
        sleep(TIEMPO_ENTRE_ACCIONES);
    }
}

int main() {
    key_t key;
    key = ftok("/tmp", 'B');
    struct shmemory* shmemory;
    int shmid = shmget(key, sizeof(struct shmemory), IPC_CREAT | 0666);
    shmemory = (struct shmemory *) shmat(shmid, NULL, 0);

    //Inicialización

    for (int i = 0; i < NUM_HORAS; i++) {
        shmemory->tabla_reservas[i] = -1;
    }
    sem_init(&shmemory->mutex_tabla, 1, 1);
    sem_init(&shmemory->consultantes, 1, 0);
    sem_init(&shmemory->acceso_consultantes, 1, 1);

    //Creación de procesos
    pid_t pid;
    for (int i = 1; i <= NUM_PROCESOS; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Error al crear el proceso hijo");
            exit(1);
        } else if (pid == 0) {
            accion_alumnos(i);
            exit(0);
        }
        usleep(400000);
    }

    for (int i = 0; i < NUM_PROCESOS; i++) {
        wait(NULL);
    }

    //Liberación de recursos
    if (sem_destroy(&shmemory->mutex_tabla) == -1 || sem_destroy(&shmemory->consultantes) == -1 || sem_destroy(&shmemory->acceso_consultantes) == -1) {
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }
    shmdt(shmemory);
    shmctl(shmid, IPC_RMID, NULL);

    printf("Fin del programa de reservas\n");
    return 0;
}
