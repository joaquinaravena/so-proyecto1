#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
void showFile(char *path) {
     FILE *file;
    if ((file = fopen(path, "r")) == NULL){
        printf("Error al abrir el archivo: %s\n", strerror(errno));
        exit(EXIT_FAILURE);     
    }
    char c = fgetc(file);
    printf("\n");
    while (c != EOF) {
        printf("%c", c);
        c = fgetc(file);
    }
    printf("\n");
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2){
		printf("Error: parámetros insuficientes!\n");
		exit(EXIT_FAILURE);
	}
    showFile(argv[1]);
    exit(EXIT_SUCCESS);
}
