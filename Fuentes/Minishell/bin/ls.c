#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
int main(int argc, char *argv[]) {
	DIR *directorio;
	struct dirent *dir;	
	if (argc ==1){
		directorio = opendir(".");
	}else{
		directorio = opendir(argv[1]);
	}
	if (directorio!=NULL) {
		while ((dir = readdir(directorio)) != NULL) {
			printf("%s\n", dir->d_name);
		}
		closedir(directorio);
	}else{
		printf("Error al abrir el directorio: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
