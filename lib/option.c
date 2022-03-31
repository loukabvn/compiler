#include <stdlib.h>
#include <getopt.h>
#include <stdarg.h>
#include "option.h"

// flot d'entrée du programme
static FILE *input;
// flot de sortie du programme
static FILE *output;

static void usage(char *prgname) {
  fprintf(stderr,
      "Usage: %s [-i input-file] [-o output-file] [-h]\n"
      "\t-i : compile the file of path 'input-file'\n"
      "\t-o : generate the code in the file of path 'output-file'\n"
      "\t-h : print this help\n"
      "If input-file or output-file are not given stdin and the file\n"
      "generated/code.asm are used instead\n",
      prgname);
}

void print(const char *format, ...) {
  va_list ap;
	va_start(ap, format);
	vfprintf(output, format, ap);
	va_end(ap);
}

void switch_input(FILE **stream) {
  *stream = input;
}

// Gestions des options : s'ils sont passés en paramètres, stocke dans input le
// flot d'entrée de Flex et dans output le flot de sortie pour la génération
// du code assembleur.
void setoptions(int argc, char *argv[]) {
  input = stdin;
  output = NULL;
  int opt;
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
      case HELP_OPT:    // -h
        usage(argv[0]);
        exit(EXIT_SUCCESS);
      case INPUT_OPT:   // -i
        input = fopen(optarg, "r");
        if (input == NULL) {
          fprintf(stderr, "An error occurred while opening the input file.\n");
          exit(EXIT_FAILURE);
        }
        break;
      case OUTPUT_OPT:  // -o
        output = fopen(optarg, "w");
        if (output == NULL) {
          fprintf(stderr, "An error occurred while opening or creating "
                          "the output file.\n");
          exit(EXIT_FAILURE);
        }
        break;
      default:    // option inconnue
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  if (output == NULL) {
    output = fopen(DEFAULT_OUTPUT, "w");
  }
}
