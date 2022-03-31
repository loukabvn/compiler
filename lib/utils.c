#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include "utils.h"

#define CONCAT(a, b)  a ## b
#define STR(a)        #a

// Format utilisé pour les fonctions de comparaisons : eq, neq, lt, gte...
#define CMP_FORMAT(op, value_on_true, value_on_false)                          \
    "\tpop bx\n"                                                               \
    "\tpop ax\n"                                                               \
    "\tconst cx,%s\n"                                                          \
    "\tconst dx," STR(value_on_true) "\n"                                      \
		"\t" STR(op) " ax,bx\n"  /* instruction de comparaison */                  \
    "\tjmpc cx\n"            /* on fait un saut si le bit c est positionné */  \
    "\tconst dx," STR(value_on_false) "\n"                                     \
    ":%s\n"                                                                    \
    "\tpush dx\n\n"

// Format alternatif utilisé pour les fonctions  lte (<=) et gt (>)
#define CMP_ALT_FORMAT(value_on_true, value_on_false)                          \
    "\tpop bx\n"                                                               \
    "\tpop ax\n"                                                               \
    "\tconst cx,%s\n"                                                          \
    "\tconst dx," STR(value_on_true) "\n"                                      \
		"\tsless ax,bx\n"        /* 1ere instruction de comparaison (<) */         \
    "\tjmpc cx\n"            /* on fait un saut si le bit c est positionné */  \
    "\tcmp ax,bx\n"          /* 2e instruction de comparaison (=) */           \
    "\tjmpc cx\n"            /* 2e saut */                                     \
    "\tconst dx," STR(value_on_false) "\n"                                     \
    ":%s\n"                                                                    \
    "\tpush dx\n\n"

#define OP_FORMAT                                                              \
    "\tpop bx\n"                                                               \
    "\tpop ax\n"                                                               \
    "\t%s ax,bx\n"                                                             \
    "\tpush ax\n\n"

// Déclaration d'une fonction de prototype : void asm_op(), et exécutant
// l'opération correspondante en assembleur
#define DECL_ASM_FUNC(op)                                                      \
    void CONCAT(asm_, op)() {                                                  \
      print(OP_FORMAT, STR(op));                                               \
    }

// Idem que DECL_ASM_FUNC mais pour les fonctions de comparaisons
#define DECL_ASM_CMP_FUNC(op, asm_instr, value_on_true, value_on_false, alt)   \
    void CONCAT(asm_, op)() {                                                  \
      char buf[BUFMAX];                                                        \
    	create_label(buf, BUFMAX, "%s:%d", STR(op), new_label_number());         \
    	print(                                                                   \
          alt ? CMP_ALT_FORMAT(value_on_true, value_on_false)                  \
          : CMP_FORMAT(asm_instr, value_on_true, value_on_false),              \
          buf, buf);                                                           \
    }

// Fonction d'erreur

void fail_with(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

// Gestion des labels

unsigned int new_label_number() {
	static unsigned int current_label_number = 0u;
	if (current_label_number == UINT_MAX) {
		fail_with("Error: maximum label number reached !\n");
	}
	return current_label_number++;
}

void create_label(char *buf, size_t buf_size, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	if (vsnprintf(buf, buf_size, format, ap) >= buf_size) {
		va_end(ap);
		fail_with("Error in label generation: size of label exceeds max. size !\n");
	}
	va_end(ap);
}

// Déclarations des fonctions appelés pour chaque opération : asm_add, asm_sub...

DECL_ASM_FUNC(add)  // asm_add
DECL_ASM_FUNC(sub)  // asm_sub
DECL_ASM_FUNC(mul)  // asm_mul
DECL_ASM_FUNC(and)  // asm_and
DECL_ASM_FUNC(or)   // asm_or

DECL_ASM_CMP_FUNC(eq, cmp, 1, 0, false)     // asm_eq
DECL_ASM_CMP_FUNC(neq, cmp, 0, 1, false)    // asm_neq
DECL_ASM_CMP_FUNC(lt, sless, 1, 0, false)   // asm_lt
DECL_ASM_CMP_FUNC(gte, sless, 0, 1, false)  // asm_gte
DECL_ASM_CMP_FUNC(lte, NULL, 1, 0, true)    // asm_lte
DECL_ASM_CMP_FUNC(gt, NULL, 0, 1, true)     // asm_gt

// Fonctions particulières : asm_div, asm_mod, asm_minus et asm_not

// on gère le cas ou il y a une division par zéro
void asm_div() {
  print("\tpop bx\n\tpop ax\n\tconst cx,err_div_zero\n"
        "\tdiv ax,bx\n\tjmpe cx\n\tpush ax\n\n");
}

// on calcule le modulo a mod b, en faisant : mod = a - b * (a / b)
void asm_mod() {
	print("\tpop bx\n\tpop ax\n\tcp cx,ax\n\tdiv ax,bx\n"
		    "\tmul ax,bx\n\tsub cx,ax\n\tpush cx\n\n");
}

// on effectue ici le complément à 2 du nombre en sommet de pile
void asm_minus() {
	print("\tpop ax\n\tnot ax\n\tconst bx,1\n\tadd ax,bx\n\tpush ax\n\n");
}

// Soit b un booléen, on utilise ici le fait que non b est equivalent à un ou
// exclusif entre b et true.
//    -> not b == xor b,1
void asm_not() {
	print("\tconst bx,1\n\tpop ax\n\txor ax,bx\n\tpush ax\n\n");
}

// Structure facilitant l'évaluation des expressions

const asm_op operations[] = {
 {
   .asm_func = asm_add,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = INT_T
 },
 {
   .asm_func = asm_sub,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = INT_T
 },
 {
   .asm_func = asm_mul,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = INT_T
 },
 {
   .asm_func = asm_div,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = INT_T
 },
 {
   .asm_func = asm_mod,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = INT_T
 },
 {
   .asm_func = asm_minus,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = INT_T
 },
 {
   .asm_func = asm_and,
   .arg1_t = BOOL_T,
   .arg2_t = BOOL_T,
   .return_t = BOOL_T
 },
 {
   .asm_func = asm_or,
   .arg1_t = BOOL_T,
   .arg2_t = BOOL_T,
   .return_t = BOOL_T
 },
 {
   .asm_func = asm_not,
   .arg1_t = BOOL_T,
   .arg2_t = BOOL_T,
   .return_t = BOOL_T
 },
 {
   .asm_func = asm_eq,
   .arg1_t = ANY_TYPE,
   .arg2_t = ANY_TYPE,
   .return_t = BOOL_T
 },
 {
   .asm_func = asm_neq,
   .arg1_t = ANY_TYPE,
   .arg2_t = ANY_TYPE,
   .return_t = BOOL_T
 },
 {
   .asm_func = asm_lt,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = BOOL_T
 },
 {
   .asm_func = asm_gt,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = BOOL_T
 },
 {
   .asm_func = asm_lte,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = BOOL_T
 },
 {
   .asm_func = asm_gte,
   .arg1_t = INT_T,
   .arg2_t = INT_T,
   .return_t = BOOL_T
 }
};
