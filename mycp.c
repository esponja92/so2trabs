#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <locale.h>


static  locale_t locale;

int MyStrerrorInit(void) {
  locale = newlocale(LC_CTYPE_MASK|LC_NUMERIC_MASK|LC_TIME_MASK|LC_COLLATE_MASK|LC_MONETARY_MASK|LC_MESSAGES_MASK,"",(locale_t)0);

  if (locale == (locale_t)0) {
    return 0;
  }

  return 1;
}


int main(int argc, char* argv[]) {

  // TODO: fazer getopt akew



  MyStrerrorInit();

  int c;
  FILE *in, *out;

  in = fopen(argv[1], "r");
  out = fopen(argv[2], "w");

  if(out == NULL) {
    // Verifica se houve erro pelo destino ser uma pasta
    if(errno == EISDIR) {
      // Splita o path de entrada para pegar o nome do arquivo original
      char *dir_file, *filename;
      // TODO: BUG strcpy dando erro, argumento 2 tem q ser constante
      strcpy(dir_file, argv[2]);
      filename = strrchr(argv[1], '/');
      strcat(dir_file, filename);

      // Usa como destino o diretorio dado concatenado com o nome do arquivo original
      out = fopen(dir_file, "w");
    } else {
      printf("mycp: não foi possível criar arquivo comum \"%s\": %s\n", argv[2], (char*) strerror_l(errno, locale));
      return 1;
    }
  }

  while ((c = fgetc(in)) != EOF)
    fputc(c, out);

  return 0;
}