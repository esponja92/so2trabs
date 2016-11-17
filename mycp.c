#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <dirent.h>
#include <sys/stat.h>


static locale_t locale;


void show_help() {
  printf("Uso: ./mycp [-R] ORIGEM... DESTINO\n");
  printf("Copia ORIGEM para DESTINO, ou multiplas ORIGENS(s) para DIRETORIO.\n");
  printf("\n");
  printf("Argumentos opicionais disponíveis:\n");
  printf("  -R, -r   copia os diretórios recursivamente\n");
  printf("  -h       mostra esta ajuda e finaliza\n");
}


int copy_file(char * in_path, char * out_str) {
  int c;
  FILE *in_file, *out_file;
  
  // Abre o arquivo a ser copiado
  in_file = fopen(in_path, "r");

  if(in_file == NULL) {
    printf("mycp: não foi possível abrir o arquivo '%s': %s\n", in_path, (char *) strerror_l(errno, locale));
    return EXIT_FAILURE;
  }

  // Prepara o path de destino
  char *out_path = (char *) calloc(strlen(out_str) + 2 + strlen(in_path), sizeof(char));
  strcpy(out_path, out_str);
  struct stat statbuf;
  
  // Obtem o filename da origem
  char *filename, *split;
  split = strrchr(in_path, '/');
  if(split == NULL) {
    filename = (char *) calloc(strlen(in_path), sizeof(char));
    strcpy(filename, in_path);
  } else {
    filename = (char *) calloc(strlen(split)-1, sizeof(char));
    strcpy(filename, split+1);
  }

  // Pega o stat do destino
  lstat(out_str, &statbuf);
  
  // Verifica se o destino é uma pasta e coloca o filename depois
  if(S_ISDIR(statbuf.st_mode)) {
    strcat(out_path, "/");
    strcat(out_path, filename);
  }

  // Abre o arquivo de destino
  out_file = fopen(out_path, "w");

  // Verifica se houve erro ao abrir arquivo de destino (geralmente problema de permissão)
  if(out_file == NULL) {
    printf("mycp: não foi possível criar arquivo comum '%s': %s\n", out_path, (char *) strerror_l(errno, locale));
    return EXIT_FAILURE;
  }

  // Copia arquivo char por char
  while ((c = fgetc(in_file)) != EOF)
    fputc(c, out_file);

  free(filename);
  free(out_path);
  fclose(in_file);
  fclose(out_file);
  return EXIT_SUCCESS;
}


int copy_folder(char *in_dir, char *out_dir) {
  DIR *dir = opendir(in_dir);
  struct dirent *ent;
  struct stat statbuf;

  if(dir != NULL) {
    while((ent = readdir(dir)) != NULL) {
      // Pula as pastas . e ..
      if(strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
        continue;

      // Pega o caminho completo até o alvo de entrada
      char *in_path = (char *) calloc(strlen(in_dir) + 2 + strlen(ent->d_name), sizeof(char));
      strcpy(in_path, in_dir);
      strcat(in_path, "/");
      strcat(in_path, ent->d_name);

      // Pega o caminho completo até o alvo de saida e o stat da pasta de origem
      lstat(in_dir, &statbuf);
      char *out_path = (char *) calloc(strlen(out_dir) + 2 + strlen(ent->d_name), sizeof(char));
      strcpy(out_path, out_dir);
      strcat(out_path, "/");
      strcat(out_path, ent->d_name);

      // Copia pasta
      if(ent->d_type == DT_DIR) {
        mkdir(out_path, statbuf.st_mode);
        copy_folder(in_path, out_path);
      } else if(ent->d_type == DT_LNK) {
        // Copia link
        int size = 200;
        char *buffer = (char *) calloc(size, sizeof(char));
        readlink(in_path, buffer, size);
        symlink(buffer, out_path);
        free(buffer);
      } else {
        // Copia arquivos em geral
        copy_file(in_path, out_dir);
      }
      free(in_path);
      free(out_path);
    }
    closedir(dir);
  } else {
    printf("mycp: não foi possível abrir o diretorio '%s': %s\n", in_dir, (char *) strerror_l(errno, locale));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


void main(int argc, char * argv[]) {
  int i;
  struct stat statbuf;

  int rflag = 0;
  int c;
  opterr = 0;
  while((c = getopt(argc, argv, "Rrh")) != -1)
    switch(c) {
      case 'R':
      case 'r':
        rflag = 1;
        break;
      case 'h':
        show_help();
        exit(EXIT_SUCCESS);
      case '?':
        printf("mycp: opção -- '%c' inválida\n", optopt);
        printf("Tente './mycp -h' para mais informações.\n");
        exit(EXIT_FAILURE);
    }

  
  if(argv[optind] == NULL) {
    printf("mycp: falta o operando arquivo\n");
    exit(EXIT_FAILURE);
  }
  if(argv[optind + 1] == NULL) {
    printf("mycp: falta o operando arquivo de destino após '%s'\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  // Inicializa informações padrões de localização
  locale = newlocale(LC_CTYPE_MASK|LC_NUMERIC_MASK|LC_TIME_MASK|LC_COLLATE_MASK|LC_MONETARY_MASK|LC_MESSAGES_MASK,"",(locale_t)0);

  // Caminho de destino
  char *dest = argv[argc - 1];

  // Pega o stat do destino
  lstat(dest, &statbuf);

  // Se for uma copia de varios arquivos de entrada, 
  // o destino tem que ser uma pasta
  if(argc > 3 && !S_ISDIR(statbuf.st_mode)) {
    printf("mycp: o alvo '%s' não é um diretório\n", dest);
    exit(1);
  }


  for(i = optind ; i < argc - 1 ; i++) {
    // Verifica se existe arquivo ou pasta e pega o stat
    if(lstat(argv[i], &statbuf) == 0) {
      // Verifica se é pasta
      if(S_ISDIR(statbuf.st_mode)) {
        // Verifica se a flag de recursão -R está ativada
        if(rflag) {
          // Remove a ultima barra se tiver
          if(argv[i][strlen(argv[i])-1] == '/')
            argv[i][strlen(argv[i])-1] = ' ';
          // Obtem o foldername
          char *foldername, *split;
          split = strrchr(argv[i], '/');
          if(split == NULL) {
            foldername = (char *) calloc(strlen(argv[i]), sizeof(char));
            strcpy(foldername, argv[i]);
          } else {
            foldername = (char *) calloc(strlen(split)-1, sizeof(char));
            strcpy(foldername, split+1);
          }
          // Recoloca a ultima barra se tiver
          if(argv[i][strlen(argv[i])-1] == ' ')
            argv[i][strlen(argv[i])-1] = '/';

          // Cria a pasta
          char *folder = (char *) calloc(strlen(dest) + 2 + strlen(foldername), sizeof(char));
          strcpy(folder, dest);
          strcat(folder, "/");
          strcat(folder, foldername);
          mkdir(folder, statbuf.st_mode);

          // Copia a pasta
          copy_folder(argv[i], folder);
        } else {
          printf("mycp: omitindo o diretório '%s'\n", argv[i]);
        }
      } else {
        // Se não copia o arquivo
        copy_file(argv[i], dest);
      }
    } else {
      printf("mycp: impossível obter estado de '%s': Arquivo ou diretório não encontrado\n", argv[i]);
    }
  }

  exit(EXIT_SUCCESS);
}