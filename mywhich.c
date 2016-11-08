#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int existeArquivoNoPath(char* caminhoDoArquivo){
	return open(caminhoDoArquivo, O_RDONLY, S_IRUSR | S_IXOTH);
}

int main(int argc, char** argv){

	char* s = getenv("PATH");
	char* path = malloc(100*sizeof(char));
	printf("s: %s\n", s);

	path = strtok(s, ":");
	printf(" primeiro strtok: %s\n", s);
	while (path != NULL){
		strcat(path, "/");
		strcat(path, argv[1]);
		printf(" buscando no %s\n", path);
		if(existeArquivoNoPath(path) != -1){
			printf("Achou aqui: %s\n", path);
		}
		path = strtok(NULL, ":");
	}
}

