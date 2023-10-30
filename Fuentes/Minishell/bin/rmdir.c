#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
int main(int argc, char *argv[]) {
	if (argc < 2){
		printf("Error: parámetros insuficientes!\n");
		exit(EXIT_FAILURE);
	}
	if (rmdir(argv[1]) == -1) {
		printf("Error al eliminar el directorio: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
    exit(EXIT_SUCCESS);
}
