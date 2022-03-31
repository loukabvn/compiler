%{
  #include <limits.h>
  #include "lib/types.h"
  #include "lib/utils.h"
  #include "mylanguage.tab.h"

  char *yy_copy;
  // Convertie une chaîne de digits en int
  void string_to_int(int *v, const char *s);
  // Stocke le contenu de yytext dans yy_copy puis dans yylval.string
  void store_string();
%}

%x COMMENT
%option noyywrap
%%
[[:digit:]]+    {
                  string_to_int(&yylval.integer, yytext);
                  return NUMBER;
                }
"true"          { return TRUE;   }
"false"         { return FALSE;  }
"and"|"&&"      { return AND;    }
"or"|"\|\|"     { return OR;     }
"not"|"!"       { return NOT;    }
"=="            { return EQ;     }
"!="            { return NEQ;    }
"<="            { return LTE;    }  // less than or equal
">="            { return GTE;    }  // greater than or equal
"++"            { return INC;    }
"--"            { return DEC;    }
"+="            { return SELF_PLUS;  }
"-="            { return SELF_MINUS; }
"*="            { return SELF_TIMES; }
"/="            { return SELF_DIV;   }
"%="            { return SELF_MOD;   }
"if"            { return IF;     }
"else"          { return ELSE;   }
"while"         { return WHILE;  }
"int"           {
                  yylval.type = INT_T;
                  return TYPE;
                }
"bool"          {
                  yylval.type = BOOL_T;
                  return TYPE;
                }
"void"          {
                  yylval.type = VOID_T;
                  return TYPE;
                }
"print"         { return PRINT;   }
"println"       { return PRINTLN; }
[[:alpha:]]+[[:alpha:]|_|[:digit:]]*  {   // un identifiant doit être une
                  store_string();         // suite de lettres, chiffres ou '_'
                  return ID;              // commençant par une lettre
                }
\"[^\"]+\"      {
                  store_string();
                  return CONST_STRING;
                }
"{"             { return OPEN_BRACKET;  }
"}"             { return CLOSE_BRACKET; }
[[:blank:]]
"//"            { BEGIN(COMMENT); }
<COMMENT>[^\n]
<COMMENT>\n     { BEGIN(INITIAL); }
.               { return yytext[0]; }
\n
%%

void store_string() {
  size_t length = strlen(yytext) + 1;
  yy_copy = malloc(length);
  strncpy(yy_copy, yytext, length);
  yylval.string = yy_copy;
}

void string_to_int(int *r, const char *s) {
  char *p;
  long v;
  errno = 0;
  v = strtol(yytext, &p, 10);
  if ((*p != '\0' || (errno == ERANGE && (v == LONG_MIN || v == LONG_MAX)))
       || (v < INT_MIN || v > INT_MAX)) {
    fprintf(stderr, "Error converting string to int\n");
    exit(EXIT_FAILURE);
  }
  *r = v;
}
