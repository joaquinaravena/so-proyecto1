#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CANT_CLIENTES 80
#define CAPACIDAD_ENTRADA 30
#define NUM_EMPLEADOS_EMP 2
#define NUM_EMPLEADOS_COM 1
#define CAPACIDAD_FILA 15
#define ESPERA 2

// Definir semáforos
sem_t cant_entrada;  // Semaforo para controlar el acceso a la mesa de entrada
sem_t fila_empresa;  // Semaforo para controlar el acceso a la fila de empresas
sem_t fila_comunes;  // Semaforo para controlar el acceso a la fila de clientes comunes
sem_t fila_politicos;  // Semaforo para dar prioridad a los políticos
sem_t empresa_esperando;  // Semaforo para controlar si hay clientes de tipo empresa esperando
sem_t comunes_esperando;  // Semaforo para controlar si hay clientes de tipo comun esperando
sem_t politicos_esperando;  // Semaforo para controlar si hay politicos esperando
sem_t gente_esperandoEmp; //Semaforo para despertar a los empleados de empresa descansando
sem_t gente_esperandoCom; //Semaforo para despertar a los empleados de comun descansando
pthread_mutex_t mutexEntrada; //Mutex para no permitir el acceso concurrente a la mesa de entrada
pthread_mutex_t mutexAtencion; //Mutex para no permitir el acceso concurrente a los clientes esperando en cada fila

typedef struct {
    int tipo;  // 0 para empresas, 1 para clientes comunes, 2 para políticos
    int id;
} Cliente;


void* mesa_de_entrada(void* arg) {
    Cliente* cliente = (Cliente*)arg;
    pthread_mutex_lock(&mutexEntrada);
    printf("Cliente %d, tipo %i, llega a la mesa de entrada.\n", cliente->id, cliente->tipo);
    if (sem_trywait(&cant_entrada) == 0) {
        // Determinar el destino del cliente
        if (cliente->tipo == 0) {  // Empresa
            pthread_mutex_unlock(&mutexEntrada);
            sem_wait(&fila_empresa); //entre a fila empresa
            printf("%i: Hay un cliente empresa esperando en fila de tipo empresa\n", cliente->id);
            sem_post(&cant_entrada);// libero de la entrada
            sem_post(&empresa_esperando);
            sem_post(&gente_esperandoEmp);
        } else if (cliente->tipo == 1) {  // Cliente común
            pthread_mutex_unlock(&mutexEntrada);
            sem_wait(&fila_comunes); //entre a fila comun
            printf("%i: Hay un cliente comun esperando en fila de tipo común\n", cliente->id);
            sem_post(&cant_entrada);// libero de la entrada
            sem_post(&comunes_esperando);
            sem_post(&gente_esperandoCom);
        } else {  // Político
            pthread_mutex_unlock(&mutexEntrada);
            sem_wait(&fila_politicos); //entre a fila politicos
            printf("%i: Hay un politico esperando en fila de tipo político\n", cliente->id);
            sem_post(&cant_entrada);// libero de la entrada
            sem_post(&politicos_esperando);
            sem_post(&gente_esperandoCom);
            sem_post(&gente_esperandoEmp);
        }
    } else {
        // El cliente se retira si la mesa de entrada está llena
        pthread_mutex_unlock(&mutexEntrada);
        printf("Cliente %d, tipo %i, se retira debido a que la mesa de entrada está llena.\n", cliente->id, cliente->tipo);
    }
    free(cliente);
    pthread_exit(NULL);
}

void* empleadoEmpresa(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutexAtencion);
        if(sem_trywait(&gente_esperandoEmp) != 0) {
            printf("Empleado Empresa descansa\n");
            pthread_mutex_unlock(&mutexAtencion);
            sem_wait(&gente_esperandoEmp);
            pthread_mutex_lock(&mutexAtencion);
        }

        if (sem_trywait(&politicos_esperando)  == 0) {
            // Atender a un político
            printf("Político siendo atendido por empleado empresa\n");
            sem_wait(&gente_esperandoCom);
            pthread_mutex_unlock(&mutexAtencion);
            sleep(ESPERA);
            sem_post(&fila_politicos);
            printf("Político termino\n");
        } else if (sem_trywait(&empresa_esperando) == 0) {
            // Atender a una empresa
            printf("Empresa atendiendo\n");
            pthread_mutex_unlock(&mutexAtencion);
            sleep(ESPERA);
            sem_post(&fila_empresa);
            printf("Empresa termino\n");
        }
    }
}

void* empleadoComun(void* arg) {
    while (1) {
        if(sem_trywait(&gente_esperandoCom) != 0) {
            printf("Empleado Comun descansa\n");
            sem_wait(&gente_esperandoCom);
        }
        pthread_mutex_lock(&mutexAtencion);
        if (sem_trywait(&politicos_esperando)  == 0) {
            // Atender a un político
            printf("Político siendo atendido por empleado comun\n");
            sem_wait(&gente_esperandoEmp);
            pthread_mutex_unlock(&mutexAtencion);
            sleep(ESPERA);
            sem_post(&fila_politicos);
            printf("Político termino\n");
        } else if (sem_trywait(&comunes_esperando) == 0) {
            // Atender a un comun
            printf("Comun atendiendo\n");
            pthread_mutex_unlock(&mutexAtencion);
            sleep(ESPERA);
            sem_post(&fila_comunes);
            printf("Comunes termino\n");
        }
    }
}



int main() {
    pthread_t empleadoE[NUM_EMPLEADOS_EMP];
    pthread_t empleadoC[NUM_EMPLEADOS_COM];
    pthread_t clientes[CANT_CLIENTES];

    // Inicializar semáforos
    sem_init(&cant_entrada, 0, CAPACIDAD_ENTRADA);
    sem_init(&fila_empresa, 0, CAPACIDAD_FILA);
    sem_init(&fila_comunes, 0, CAPACIDAD_FILA);
    sem_init(&fila_politicos, 0, CAPACIDAD_FILA);
    sem_init(&empresa_esperando, 0, 0);
    sem_init(&comunes_esperando, 0, 0);
    sem_init(&politicos_esperando, 0, 0);
    sem_init(&gente_esperandoCom, 0, 0);
    sem_init(&gente_esperandoEmp, 0, 0);
    pthread_mutex_init(&mutexEntrada, NULL);
    pthread_mutex_init(&mutexAtencion, NULL);

    // Crear hilos para empleados
    for (int i = 0; i < NUM_EMPLEADOS_EMP; i++) {
        pthread_create(&empleadoE[i], NULL, empleadoEmpresa, NULL);
    }
    for (int i = 0; i < NUM_EMPLEADOS_COM; i++) {
        pthread_create(&empleadoC[i], NULL, empleadoComun, NULL);
    }
    sleep(ESPERA);
    // Crear hilos para clientes
    for (int i = 0; i < CANT_CLIENTES; i++) {
        int tipo = rand() % 3; // 0: Empresa, 1: Cliente común, 2: Político
        Cliente *cliente = (Cliente *) malloc(sizeof(Cliente));
        cliente->tipo = tipo;
        cliente->id = i + 1;
        pthread_create(&clientes[i], NULL, mesa_de_entrada, cliente);
        sleep(1); // Simula la llegada de clientes en intervalos
    }

    // Esperar a que terminen todos los hilos de empleados y clientes
    for (int i = 0; i < CANT_CLIENTES; i++) {
        pthread_join(clientes[i], NULL);
    }
    for (int i = 0; i < NUM_EMPLEADOS_EMP; i++) {
        pthread_join(empleadoE[i], NULL);
    }
    for (int i = 0; i < NUM_EMPLEADOS_COM; i++) {
        pthread_join(empleadoC[i], NULL);
    }


    // Destruir los semáforos
    sem_destroy(&cant_entrada);
    sem_destroy(&fila_empresa);
    sem_destroy(&fila_comunes);
    sem_destroy(&fila_politicos);
    sem_destroy(&empresa_esperando);
    sem_destroy(&comunes_esperando);
    sem_destroy(&politicos_esperando);
    sem_destroy(&gente_esperandoEmp);
    sem_destroy(&gente_esperandoCom);
    pthread_mutex_destroy(&mutexEntrada);
    pthread_mutex_destroy(&mutexAtencion);

    return 0;
}
