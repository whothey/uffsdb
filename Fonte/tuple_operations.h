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
 * Verifica se a tupla informada atende uma condição de qr_filter
 */
int tupleIsValid(char *data, tp_table *schema, qr_filter *condition);

/**
 * Cria o Schema (tp_table) á artir de um qr_select, onde este
 * "concatenará" os esquemas de todos as tabelas envolvidas neste
 * select, iniciando pela OUTER TABLE (from), e preenchendo
 * sequencialmente com as tabelas dos JOINs
 */
tp_table *createFullSchema(qr_select *select);

/** 
 * Transforma uma tupla (data), baseado em um schema em um
 * encadeamento de estruturas *column
 */
column *composeTuple(char *tuple, tp_table *schema);

#endif /* _UFFSDB_TUPLEOP_ */
