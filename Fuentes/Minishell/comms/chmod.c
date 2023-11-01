#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
int main(int argc, char *argv[]){
    int i;
    if (argc < 3){
	printf("Error: parámetros insuficientes!\n");
	exit(EXIT_FAILURE);
    }
    i = strtol(argv[2], 0, 8);
    if (chmod (argv[1],i) < 0){
        printf("Error al modificar los permisos del archivo: %s\n", strerror(errno));
        exit(EXIT_FAILURE);     
    }
    exit(EXIT_SUCCESS);
}
