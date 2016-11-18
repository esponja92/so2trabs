#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int existeArquivoNoPath(char* caminhoDoArquivo, char* nome_do_arquivo){
	char str[1000] = "";
	strcat(str, caminhoDoArquivo);
	strcat(str, "/");
	strcat(str, nome_do_arquivo);
	int open_return_value =  open(str, O_RDONLY, S_IRUSR | S_IXOTH);

	return open_return_value;
	
}

int main(int argc, char** argv){

	char* s = getenv("PATH");
	char* path = (char*)malloc(100*sizeof(char));

	path = strtok(s, ":");
	while (path != NULL){
		if(existeArquivoNoPath(path, argv[1]) != -1){
			strcat(path, "/");
			strcat(path, argv[1]);
			printf("%s\n", path);
			break;
		}
		path = strtok(NULL, ":");
	}
}

