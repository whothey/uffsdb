#ifndef _UFFSDB_JOIN_
#define _UFFSDB_JOIN_

#include "buffend.h"
#include "tuple_operations.h"
#include <string.h>

typedef struct _join_data {
  char      *table_name;
  tp_table  *schema;
  qr_filter *condition;
  int        join_type;
  int        current_index;
  struct fs_objects object;
  struct _join_data *next;
  struct _join_data *prev;
} tp_join;

/**
 * Cria as estruturas de tp_join basedas na estrutura qr_join
 * recursivamente.
 */
tp_join *composeJoinData(qr_join *query_data, int qtt);

/**
 * Reseta todos os índices de iteração dos JOINs
 */
void resetJoinIndexes(tp_join *joins);

/**
 * JOIN uma tupla específica com a próxima iteração de JOINS definido
 * em *join_data.
 * ESTA FUNÇÃO NÃO VERIFICA SE A TUPLA SATISFAZ A OPERAÇÃO DE JOIN!
 * Um schema-resultado deve ser informado para a operação.
 */
char *joinNext(char *tuple, tp_join *join_data, tp_table *completeSchema);

#endif /* _UFFSDB_JOIN_ */
