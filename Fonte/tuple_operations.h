#ifndef _UFFSDB_TUPLEOP_
#define _UFFSDB_TUPLEOP_

#include "buffend.h"
#include <string.h>

/**
 * Arquivo voltado para operações com tuplas
 */

/**
 * Retorna o número de colunas de um schema
 */
int schema_column_count(tp_table *schema);

/**
 * Retorna o tamanho em bytes de um schema (tp_table)
 */
int schema_row_bytesize(tp_table *schema);

/**
 * Cria o Schema (tp_table) á artir de um qr_select, onde este
 * "concatenará" os esquemas de todos as tabelas envolvidas neste
 * select, iniciando pela OUTER TABLE (from), e preenchendo
 * sequencialmente com as tabelas dos JOINs
 */
tp_table *createSelectSchema(qr_select *select);

/**
 * O mesmo que createSelectSchema, porém sempre considera que a
 * projeção informada é '*'
 */
tp_table *createFullSchema(qr_select *select);

/**
 * Transforma uma tupla (data), baseado em um schema em um
 * encadeamento de estruturas *column;
 */
column *composeTuple(char *tuple, tp_table *schema);

/**
 * Cria *list_values para comparação de estruturas column
 */
list_value *columnListValues(column* data, qr_filter *condition);

#endif /* _UFFSDB_TUPLEOP_ */
