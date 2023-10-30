#include <stdio.h>
#include <stdlib.h>
int main(){
	printf("\nEstos son los comandos definidos: \n\n");
	printf("  mkdir <nombre_directorio> -> Crea un directorio con el nombre <nombre_directorio>.\n\n");
	printf("  rmdir <nombre_directorio> -> Elimina el directorio con el nombre <nombre_directorio>.\n\n");
	printf("  touch <nombre_archivo> -> Crea un archivo con el nombre <nombre_archivo>.\n\n");
	printf("  ls <nombre_directorio> -> Lista el contenido del directorio con el nombre <nombre_directorio>.\n\n");
	printf("  cat <nombre_archivo> -> Muestra el contenido del archivo con el nombre <nombre_archivo>.\n\n");
	printf("  chmod <nombre_archivo> <permisos> -> Cambia los permisos <permisos> del archivo con el nombre <nombre_archivo>.\n\n");
	printf("  clear-> Limpia la pantalla de la terminal\n\n");
	printf("  help-> Muestra ayuda con los comandos posibles\n\n");
	printf("  exit-> Finaliza la ejecución de la terminal\n\n");	
	return 1;
}
