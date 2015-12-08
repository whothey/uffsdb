#include "queryer.h"

/* Estrutura global que guarda as informações obtidas pelo yacc
 * na identificação dos tokens
 */
rc_insert GLOBAL_DATA;

/* Estrutura auxiliar do reconhecedor.
 */
rc_parser GLOBAL_PARSER;

void start_select()
{
  GLOBAL_SELECT.nprojection = 0;
  free(GLOBAL_SELECT.projection);
  free(GLOBAL_SELECT.tables);
  free(GLOBAL_SELECT.filters);
  free(GLOBAL_SELECT.join);
}

int add_column_to_projection(char **args)
{
  char **temp;
  char *column = args[0];
  
  printf("%s", column);
  temp = (char **) realloc(GLOBAL_SELECT.projection, sizeof(GLOBAL_SELECT.projection) + sizeof(column));

  if (temp == NULL) {
    fprintf(stderr, "Cannot realloc projection status to SELECT");
    GLOBAL_PARSER.noerror=0;
    return 0;
  }

  GLOBAL_SELECT.nprojection++;
  
  strcpy(GLOBAL_SELECT.projection[GLOBAL_SELECT.nprojection - 1], column);

  return 1;
}
