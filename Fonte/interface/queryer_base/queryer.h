/**
 * Queryer lib
 * 
 * Queryer é a biblioteca de interpretação do SELECT do UFFSDB, ela é
 * utilizada pelo YaCC para a definição da estrutura básica para
 * consulta do SELECT.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "y.tab.h"
#include "parser.h"

/**
 * qr_filters
 * Guarda os filtros (wheres) á serem executados pela consulta
 *
 * Uma boa maneira de pensar, caso a estrutura tenha ficado confusa é
 * que ela está implementada para SELECTS, exemplo:
 * SELECT * FROM tabela1 WHERE col1  =   2    AND    col2  =  'A' OR col3 = 'B';
 * Posição na estrutura:      (left op right logic   left op right...)
 */
typedef struct qr_filter {
  int logicOperator; // Operador lógico em relação á outras comparações, constantes OP_AND e OP_OR
  char *left;        // Operador da esquerda do filtro (coluna)
  char op;           // Operador lógico da comparação (>, <, =, <>...)
  char *right;       // Operador da direita do filtro
} qr_filter;

typedef struct qr_join {
  char *table;
  qr_filter condition;
} qr_join;

/**
 * qr_select
 *
 * Estrutura para gerenciar os termos interpretados pelo YaCC e
 * facilitar o acesso em nível da consulta.
 */
typedef struct qr_select {
  char **projection;  // Projeção do select
  int nprojection;
  char *tables;       // Tabelas envolvidas no SELECT
  qr_filter *filters; // Filtros (WHEREs)
  qr_join *join;      // Join -- será implementado posteriormente
} qr_select;

extern qr_select GLOBAL_SELECT;

/** 
 * Restarts the global select state
 */
void start_select();

/** 
 *
 */
int add_column_to_projection(char **column);

int set_select_table(char* table);

int add_filter_to_select(int logic, char* left, char op, char *right);
