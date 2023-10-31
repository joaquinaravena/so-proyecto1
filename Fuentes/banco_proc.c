#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>


#define CANT_CLIENTES 10
#define CAPACIDAD_ENTRADA 30
#define NUM_EMPLEADOS_EMP 2
#define NUM_EMPLEADOS_COM 1
#define CAPACIDAD_FILA 15
#define ESPERA 2
//Definir tipos de mensajes
#define MESA_ENTRADA 0
#define FILA_EMP 1
#define FILA_COM 2
#define FILA_POL 3
#define ESPERA_EMP 4
#define ESPERA_COM 5
#define ESPERA_POL 6
#define EMPLEADO_EMP 7
#define EMPLEADO_COM 8
#define ACCESO_ENTRADA 9
#define ACCESO_CLIENTES 10
#define ATIENDE_EMP 11
#define ATIENDE_COM 12
#define ATIENDE_POL 13
#define TERMINO_ATENCION 14


typedef struct {
    int tipo;  // 0 para empresas, 1 para clientes comunes, 2 para políticos
    int id;
} Cliente;

struct message_buffer {
    long message_type;
};

void mesa_de_entrada(void* arg) {
    key_t key;
    key = ftok("/tmp", 'A');
    int message_id = msgget(key, 0666 );
    struct message_buffer message;
    Cliente* cliente = (Cliente*)arg;
    msgrcv(message_id, &message, sizeof(message) - sizeof(long), ACCESO_ENTRADA, 0);
    printf("Cliente %d, tipo %i, llega a la mesa de entrada.\n", cliente->id, cliente->tipo);

    if(msgrcv(message_id, &message, sizeof (message) - sizeof(long), MESA_ENTRADA, IPC_NOWAIT) != -1){
        // Determinar el destino del cliente
        if (cliente->tipo == 0) {  // Empresa
            message.message_type = ACCESO_ENTRADA;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);

            msgrcv(message_id, &message, sizeof(message) - sizeof(long), FILA_EMP, 0); //entre a fila empresa
            printf("%i: Hay un cliente empresa esperando en fila de tipo empresa\n", cliente->id);

            message.message_type = MESA_ENTRADA;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0); // libero de la entrada
            message.message_type = ESPERA_EMP;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = EMPLEADO_EMP;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            msgrcv(message_id,&message,sizeof (message)-sizeof (long),ATIENDE_EMP,0);
            printf("Empresa %d está siendo atendido\n", cliente->id);
            msgrcv(message_id,&message,sizeof (message)-sizeof (long),TERMINO_ATENCION,0);
            printf("Empresa %d se retira\n", cliente->id);
        } else if (cliente->tipo == 1) {  // Cliente común
            message.message_type = ACCESO_ENTRADA;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);

            msgrcv(message_id, &message, sizeof(message) - sizeof(long), FILA_COM, 0); //entre a fila comun
            printf("%i: Hay un cliente comun esperando en fila de tipo común\n", cliente->id);

            message.message_type = MESA_ENTRADA;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0); // libero de la entrada
            message.message_type = ESPERA_COM;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = EMPLEADO_COM;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            msgrcv(message_id,&message,sizeof (message)-sizeof (long),ATIENDE_COM,0);
            printf("Común %d está siendo atendido\n", cliente->id);
            msgrcv(message_id,&message,sizeof (message)-sizeof (long),TERMINO_ATENCION,0);
            printf("Común %d se retira\n", cliente->id);
        } else {  // Político
            message.message_type = ACCESO_ENTRADA;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);

            msgrcv(message_id, &message, sizeof(message) - sizeof(long), FILA_POL, 0); //entre a fila politicos
            printf("%i: Hay un politico esperando en fila de tipo político\n", cliente->id);

            message.message_type = MESA_ENTRADA;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0); // libero de la entrada
            message.message_type = ESPERA_POL;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = EMPLEADO_COM;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = EMPLEADO_EMP;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            msgrcv(message_id,&message,sizeof (message)-sizeof (long),ATIENDE_POL,0);
            printf("Politico %d está siendo atendido\n", cliente->id);
            msgrcv(message_id,&message,sizeof (message)-sizeof (long),TERMINO_ATENCION,0);
            printf("Politico %d se retira\n", cliente->id);
        }
    } else {
        // El cliente se retira si la mesa de entrada está llena
        message.message_type = ACCESO_ENTRADA;
        msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
        printf("Cliente %d, tipo %i, se retira debido a que la mesa de entrada está llena.\n", cliente->id, cliente->tipo);
    }
    free(cliente);
}

void empleadoEmpresa() {
    key_t key;
    key = ftok("/tmp", 'A');
    int message_id = msgget(key, 0666 );
    struct message_buffer message;
    while (1) {
        msgrcv(message_id, &message, sizeof(message) - sizeof(long), ACCESO_CLIENTES, 0);
        if(msgrcv(message_id, &message, sizeof(message) - sizeof(long), EMPLEADO_EMP, IPC_NOWAIT) == -1){
            printf("Empleado Empresa descansa\n");
            message.message_type = ACCESO_CLIENTES;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            msgrcv(message_id, &message, sizeof(message) - sizeof(long), EMPLEADO_EMP, 0);
            msgrcv(message_id, &message, sizeof(message) - sizeof(long), ACCESO_CLIENTES, 0);
        }
        if (msgrcv(message_id, &message, sizeof(message) - sizeof(long), ESPERA_POL, IPC_NOWAIT) != -1) {
            // Atender a un político
            message.message_type = ATIENDE_POL;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            msgrcv(message_id, &message, sizeof(message) - sizeof(long), EMPLEADO_COM, 0);
            message.message_type = ACCESO_CLIENTES;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            sleep(ESPERA);
            message.message_type = FILA_POL;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = TERMINO_ATENCION;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
        } else if (msgrcv(message_id, &message, sizeof(message) - sizeof(long), ESPERA_EMP, IPC_NOWAIT) != -1) {
            // Atender a una empresa
            message.message_type = ATIENDE_EMP;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = ACCESO_CLIENTES;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            sleep(ESPERA);
            message.message_type = FILA_EMP;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = TERMINO_ATENCION;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
        }
    }
}

void empleadoComun() {
    key_t key;
    key = ftok("/tmp", 'A');
    int message_id = msgget(key, 0666 );
    struct message_buffer message;
    while (1) {
        if(msgrcv(message_id, &message, sizeof(message) - sizeof(long), EMPLEADO_COM, IPC_NOWAIT) == -1) {
            printf("Empleado Comun descansa\n");
            msgrcv(message_id, &message, sizeof(message) - sizeof(long), EMPLEADO_COM, 0);
        }
        msgrcv(message_id, &message, sizeof(message) - sizeof(long), ACCESO_CLIENTES, 0);
        if (msgrcv(message_id, &message, sizeof(message) - sizeof(long), ESPERA_POL, IPC_NOWAIT) != -1) {
            // Atender a un político
            message.message_type = ATIENDE_POL;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            msgrcv(message_id, &message, sizeof(message) - sizeof(long), EMPLEADO_EMP, 0);
            message.message_type = ACCESO_CLIENTES;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            sleep(ESPERA);
            message.message_type = FILA_POL;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = TERMINO_ATENCION;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
        } else if (msgrcv(message_id, &message, sizeof(message) - sizeof(long), ESPERA_COM, IPC_NOWAIT) != -1) {
            // Atender a un comun
            message.message_type = ATIENDE_COM;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = ACCESO_CLIENTES;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            sleep(ESPERA);
            message.message_type = FILA_COM;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
            message.message_type = TERMINO_ATENCION;
            msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
        }
    }
}



int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    // Crear la cola de mensajes y obtener su identificador
    key_t key = ftok("/tmp", 'A');
    int message_id = msgget(key, 0666 | IPC_CREAT);
    if(message_id != -1){
        msgctl(message_id, IPC_RMID, NULL);
        message_id = msgget(key, 0666 | IPC_CREAT);
    }
    struct message_buffer message;
    //Inicialización colas de mensajes
    message.message_type = MESA_ENTRADA;
    for(int i = 0; i < CAPACIDAD_ENTRADA; i++){
        msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
    }
    for(int i = 0; i < CAPACIDAD_FILA; i++){
        message.message_type = FILA_EMP;
        msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
        message.message_type = FILA_COM;
        msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
        message.message_type = FILA_POL;
        msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
    }
    message.message_type = ACCESO_ENTRADA;
    msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);
    message.message_type = ACCESO_CLIENTES;
    msgsnd(message_id, &message, sizeof(message) - sizeof(long), 0);


    // Crear procesos para empleados
    pid_t pid;
    pid_t pids[NUM_EMPLEADOS_COM+NUM_EMPLEADOS_EMP];
    for (int i = 0; i < NUM_EMPLEADOS_EMP; i++) {
        pids[i+1] = fork();
        if (pids[i+1] < 0) {
            fprintf(stderr, "Error al crear el proceso empresa\n");
            return 1;
        } else if (pids[i+1] == 0) {
            // Código que ejecutará el proceso hijo
            empleadoEmpresa();
            return 0;
        }
    }

    for (int i = 0; i < NUM_EMPLEADOS_COM; i++) {
        pids[0] = fork();
        if (pids[0] < 0) {
            fprintf(stderr, "Error al crear el proceso común\n");
            return 1;
        } else if (pids[0] == 0) {
            // Código que ejecutará el proceso hijo
            empleadoComun();
            return 0;
        }
    }
    sleep(ESPERA);
    // Crear procesos para clientes
    for (int i = 0; i < CANT_CLIENTES; i++) {
        int tipo = rand() % 3; // 0: Empresa, 1: Cliente común, 2: Político
        Cliente *cliente = (Cliente *) malloc(sizeof(Cliente));
        cliente->tipo = tipo;
        cliente->id = i + 1;
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Error al crear el proceso cliente\n");
            return 1;
        } else if (pid == 0) {
            // Código que ejecutará el proceso hijo
            mesa_de_entrada(cliente);
            return 0;
        }
        //sleep(1); // Simula la llegada de clientes en intervalos
    }
    for (int i = 0; i < CANT_CLIENTES; i++) {
        wait(NULL);
    }
    msgrcv(message_id, &message, sizeof(message) - sizeof(long), ACCESO_CLIENTES, 0);
    for(int i = 0; i < NUM_EMPLEADOS_COM+NUM_EMPLEADOS_EMP; i++){
        kill(pids[i], SIGKILL); //mato los procesos de los empleados
    }
    //Destruir cola
    if (msgctl(message_id, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la cola de mensajes");
        exit(1);
    }
    printf("No quedan clientes, cierra el banco\n");
    return 0;
}
