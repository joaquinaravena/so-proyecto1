#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CANT_CLIENTES 150
#define CAPACIDAD_ENTRADA 30
#define NUM_EMPLEADOS 3
#define CAPACIDAD_FILA 15

// Definir semáforos
sem_t cant_entrada;  // Semaforo para controlar el acceso a la mesa de entrada
sem_t fila_empresa;  // Semaforo para controlar el acceso a la fila de empresas
sem_t fila_comunes;  // Semaforo para controlar el acceso a la fila de clientes comunes
sem_t fila_politicos;  // Semaforo para dar prioridad a los políticos
sem_t empresa_esperando;  // Semaforo para controlar si hay clientes de tipo empresa esperando
sem_t comunes_esperando;  // Semaforo para controlar si hay clientes de tipo comun esperando
sem_t politicos_esperando;  // Semaforo para controlar si hay politicos esperando

typedef struct {
    int tipo;  // 0 para empresas, 1 para clientes comunes, 2 para políticos
    int id;
} Cliente;

Cliente crear_cliente(int tipo, int id) {
    Cliente cliente;
    cliente.tipo = tipo;
    cliente.id = id;
    return cliente;
}

void* mesa_de_entrada(void* arg) {
    Cliente cliente = *(Cliente*)arg;
    if (sem_trywait(&cant_entrada) == 0) {
        // Determinar el destino del cliente
        if (cliente.tipo == 0) {  // Empresa
            sem_wait(&fila_empresa); //entre a fila empresa
            sem_post(&cant_entrada);// libero de la entrada
            sem_post(&empresa_esperando);
            printf("Hay un cliente empresa esperando\n");
        } else if (cliente.tipo == 1) {  // Cliente común
            sem_wait(&fila_comunes); //entre a fila comun
            sem_post(&cant_entrada);// libero de la entrada
            sem_post(&comunes_esperando);
            printf("Hay un cliente comun esperando\n");
        } else {  // Político
            sem_wait(&fila_politicos); //entre a fila politicos
            sem_post(&cant_entrada);// libero de la entrada
            sem_post(&politicos_esperando);
            printf("Hay un politico esperando\n");
        }
    } else {
        // El cliente se retira si la mesa de entrada está llena
        printf("Cliente %d, tipo %i, se retira debido a que la mesa de entrada está llena.\n", cliente.id, cliente.tipo);
    }
    pthread_exit(NULL);
}

void* empleadoEmpresa(void* arg) {
    while (1) {
        if (sem_trywait(&politicos_esperando)  == 0) {
            // Atender a un político
            printf("Político siendo atendido por empleado empresa\n");
            sleep(2);
            sem_post(&fila_politicos);
            printf("Político termino\n");
        } else if (sem_trywait(&empresa_esperando) == 0) {
            // Atender a una empresa
            printf("Empresa atendiendo\n");
            sleep(2);
            sem_post(&fila_empresa);
            printf("Empresa termino\n");
        } else {
            // No hay clientes, el empleado descansa
            printf("Empleado Empresa descansa\n");
            sleep(2); // Simula el tiempo de descanso del empleado
        }
    }
}

void* empleadoComun(void* arg) {
    while (1) {
        if (sem_trywait(&politicos_esperando)  == 0) {
            // Atender a un político
            printf("Político siendo atendido por empleado comun\n");
            sleep(2);
            sem_post(&fila_politicos);
            printf("Político termino\n");
        } else if (sem_trywait(&comunes_esperando) == 0) {
            // Atender a un comun
            printf("Comun atendiendo\n");
            sleep(2);
            sem_post(&fila_comunes);
            printf("Comunes termino\n");
        } else {
            // No hay clientes, el empleado descansa
            printf("Empleado Comun descansa\n");
            sleep(2); // Simula el tiempo de descanso del empleado
        }
    }
}



int main() {
    pthread_t empleadoE[NUM_EMPLEADOS-1];
    pthread_t empleadoC;
    pthread_t clientes[CANT_CLIENTES];

    // Inicializar semáforos
    sem_init(&cant_entrada, 0, CAPACIDAD_ENTRADA);
    sem_init(&fila_empresa, 0, CAPACIDAD_FILA);
    sem_init(&fila_comunes, 0, CAPACIDAD_FILA);
    sem_init(&fila_politicos, 0, CAPACIDAD_FILA);
    sem_init(&empresa_esperando, 0, 0);
    sem_init(&comunes_esperando, 0, 0);
    sem_init(&politicos_esperando, 0, 0);

    // Crear hilos para empleados
    for (int i = 0; i < NUM_EMPLEADOS-1; i++) {
        pthread_create(&empleadoE[i], NULL, empleadoEmpresa, NULL);
    }
    pthread_create(&empleadoC, NULL, empleadoComun, NULL);

    // Crear hilos para clientes
    for (int i = 0; i < CANT_CLIENTES; i++) {
        int tipo = rand() % 3; // 0: Empresa, 1: Cliente común, 2: Político
        Cliente cliente = crear_cliente(tipo, i+1);
        printf("%i: generado tipo %i \n", i+1, tipo);
        pthread_create(&clientes[i], NULL, mesa_de_entrada, &cliente);
        sleep(1); // Simula la llegada de clientes en intervalos aleatorios
    }

    // Esperar a que terminen todos los hilos de empleados y clientes
    for (int i = 0; i < NUM_EMPLEADOS-1; i++) {
        pthread_join(empleadoE[i], NULL);
    }
    pthread_join(empleadoC, NULL);
    for (int i = 0; i < CANT_CLIENTES; i++) {
        pthread_join(clientes[i], NULL);
    }

    // Destruir los semáforos
    sem_destroy(&cant_entrada);
    sem_destroy(&fila_empresa);
    sem_destroy(&fila_comunes);
    sem_destroy(&fila_politicos);
    sem_destroy(&empresa_esperando);
    sem_destroy(&comunes_esperando);
    sem_destroy(&politicos_esperando);
    return 0;
}
