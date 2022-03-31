#ifndef _OPTION_H
#define _OPTION_H

#include <stdio.h>

#define OPTIONS     "hi:o:"  // chaîne correspondant aux options pour getopt()
#define INPUT_OPT   'i'     // -i input-file
#define OUTPUT_OPT  'o'     // -o output-file
#define HELP_OPT    'h'     // -h

#define DEFAULT_OUTPUT "generated/code.asm"

extern void print(const char *format, ...);
extern void switch_input(FILE **stream);
extern void setoptions(int argc, char *argv[]);

#endif
