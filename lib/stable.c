/*
  Copyright 2016 Nicolas Bedon
  This file is part of CASIPRO.

  CASIPRO is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CASIPRO is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with CASIPRO.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "option.h"

static symbol_table_entry *symbol_table = NULL;

symbol_table_entry *search_symbol_table(const char *name, symbol_class class) {
  symbol_table_entry *ste = symbol_table;
  if (class == FUNCTION) {
    for (ste = symbol_table;
        ste != NULL && (strcmp(ste->name, name) || ste->class < class);
        ste = ste->next);
  } else {
    for (ste = symbol_table;
        ste != NULL && (strcmp(ste->name, name) || ste->class == FUNCTION);
        ste = ste->next);
  }
  // fprintf(stderr, "search for : %s, of class : %d\n", name, class);
  return ste;
}

symbol_table_entry *new_symbol_table_entry(
    const char *name, const char *init_value, symbol_class class, types type) {
  symbol_table_entry *n;
  if ((n = malloc(sizeof(symbol_table_entry))) == NULL) {
    fail_with("new_symbol_table_entry: %s", strerror(errno));
  } else {
    if ((n->name = malloc(strlen(name) + 1)) == NULL) {
      fail_with("new_symbol_table_entry: %s", strerror(errno));
    } else {
      strcpy((char *)(n->name), name);
      n->init_value = init_value;
      n->desc[0] = type;
      n->class = class;
      // Last declared symbol needs to be first in the list,
      // in order to have the scope of local variables correctly
      // taken into account
      n->next = symbol_table;
      symbol_table = n;
    }
  }
  return n;
}

void load_vars() {
  print("; Zone des variables :\n");
  symbol_table_entry *ste = symbol_table;
  while (ste != NULL) {
    if (ste->class != FUNCTION) {
      const char *type = ste->desc[0] == STRING_T ? "string" : "int";
      print(":var:%s\n@%s %s\n\n", ste->name, type, ste->init_value);
    }
    ste = ste->next;
  }
}

void free_first_symbol_table_entry(void) {
  symbol_table_entry *ste = symbol_table->next;
  free(symbol_table);
  symbol_table = ste;
}
