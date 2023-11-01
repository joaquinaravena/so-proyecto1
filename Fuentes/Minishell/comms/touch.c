#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
void create_file(char *path) {
    FILE *f;
    if ((f = fopen(path, "w")) == NULL){
        printf("Error al crear el archivo: %s\n", strerror(errno));
	exit(EXIT_FAILURE);     
    }
    fclose(f);
}
int main(int argc, char *argv[]) {
	if (argc < 2){
		printf("Error: parámetros insuficientes!\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 1; i < argc; i++) {
            create_file(argv[i]);
    }
    exit(EXIT_SUCCESS);
}

