#ifndef LABEL__H
#define LABEL__H

#include "types.h"
#include "option.h"

// Constantes de textes pour l'assembleur

#define START_TEXT                                                             \
    "; Programme assembleur ASIPRO\n\n"                                        \
    "\tconst ax,debut\n"                                                       \
    "\tjmp ax\n"

#define CONST_DECL                                                             \
    "; Déclaration des constantes\n"                                           \
    ":div_by_zero\n"                                                           \
    "@string \"Error : division by zero\\n\"\n"                                \
    ":newline\n"                                                               \
    "@string \"\\n\"\n"                                                        \
    ":true\n"                                                                  \
    "@string \"true\"\n"                                                       \
    ":false\n"                                                                 \
    "@string \"false\"\n"

#define ERROR_FLAGS                                                            \
    "; Gestions des erreurs\n"                                                 \
    ":err_div_zero\n"                                                          \
    "\tconst ax,div_by_zero\n"                                                 \
    "\tcallprintfs ax\n"                                                       \
    "\tend\n"

#define START_FLAG  ":debut\n"

#define BEGIN_PRG                                                              \
    "; Préparation de la pile\n"                                               \
    "\tconst bp,pile\n"                                                        \
    "\tconst sp,pile\n"                                                        \
    "\tconst ax,2\n"                                                           \
    "\tsub sp,ax\n"                                                            \
    "\tconst ax,main\n"                                                        \
    "\tcall ax\n"

#define END_TEXT "\tend\n"

#define STACK_DECL                                                             \
    "; La zone de pile\n"                                                      \
    ":pile\n"                                                                  \
    "@int 0\n"

// Début et fin des programmes assembleur

#define START() print("%s\n%s\n%s\n", START_TEXT, CONST_DECL, ERROR_FLAGS)

#define END() print("%s\n%s\n%s\n", START_FLAG, BEGIN_PRG, END_TEXT)

#define STACK_ZONE() print(STACK_DECL)

// Instructions assembleur

#define PRINT_NEWLINE_FORMAT                                                   \
    "\tconst ax,newline\n"                                                     \
    "\tcallprintfs ax\n"

#define PRINT_INT_FORMAT                                                       \
    "\tcp ax,sp\n"                                                             \
    "\tcallprintfd ax\n"                                                       \
    "\tpop ax\n"

#define PRINT_BOOL_FORMAT                                                      \
    "\tpop ax\n"                                                               \
    "\tconst bx,0\n"                                                           \
    "\tconst cx,false\n"                                                       \
    "\tconst dx,is:false:%u\n"                                                 \
    "\tcmp ax,bx\n"                                                            \
    "\tjmpc dx\n"                                                              \
    "\tconst cx,true\n"                                                        \
    ":is:false:%u\n"                                                           \
    "\tcallprintfs cx\n"

#define PRINT_STRING_FORMAT                                                    \
    "\tconst ax,var:%s\n"                                                      \
    "\tcallprintfs ax\n"                                                       \
    "\tpop ax\n"

#define PRINT_CONST_STRING(id) print(PRINT_STRING_FORMAT, id)

#define PRINT_BOOL(number) print(PRINT_BOOL_FORMAT, number, number)

#define PRINT_INT() print(PRINT_INT_FORMAT)

#define PRINT_NEWLINE() print(PRINT_NEWLINE_FORMAT)

#define PUSH(x) print("\tconst ax,%d\n\tpush ax\n\n", x)

#define STORE_VALUE(id)                                                        \
    print("\tpop ax\n\tconst bx,var:%s\n\tstorew ax,bx\n\n", id)

#define LOAD_VALUE(id)                                                         \
    print("\tconst bx,var:%s\n\tloadw ax,bx\n\tpush ax\n\n", id)

// Instructions

#define PUT_LABEL(label) print(":%s\n", label)

#define START_COND(label)                                                      \
    print("\tpop ax\n\tconst bx,0\n\tconst cx,%s\n"                            \
          "\tcmp ax,bx\n\tjmpc cx\n", label)

#define END_COND(label)                                                        \
    print("\tconst cx,%s\n\tjmp cx\n", label)

// --- Fonctions ---

#define RETURN() print("\tret\n\n")


// Taille max du buffer pour les labels
#define BUFMAX          256
#define STACK_SIZE_MAX  64

// Nouveau numéro de label
extern unsigned int new_label_number();

// Crée un nouveau label
extern void create_label(char *buf, size_t buf_size, const char *format, ...);

// Message en cas d'erreur
extern void fail_with(const char *format, ...);

// Structure facilitant l'évaluation des expressions
typedef struct asm_op {
  void (*asm_func)();   /* code assembleur                */
  types arg1_t;         /* type du premier paramètre      */
  types arg2_t;         /* type du second paramètre       */
  types return_t;       /* type de retour de l'opération  */
} asm_op;

// Opérations disponibles
typedef enum {
  // op. sur les entiers
  ADD_OP, SUB_OP, MUL_OP, DIV_OP, MOD_OP, MINUS_OP,
  // op. booléennes
  AND_OP, OR_OP, NOT_OP, EQ_OP, NEQ_OP, LT_OP, GT_OP, LTE_OP, GTE_OP,
  MAX_OP
} operation;

// Tableau constant d'opérations de type asm_op
// Pour appeler la fonction, utiliser : operations[OP].asm_func()
extern const asm_op operations[MAX_OP];

#endif
