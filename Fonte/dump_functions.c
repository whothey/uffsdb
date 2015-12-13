#include "dump_functions.h"

void dump_columns(column *column)
{
  struct column *p = NULL;

  printf("==============COLUMNS==============\n");
  
  for (p = column; p != NULL; p = p->next) {
    printf("------------------------------------\n");
    printf("Tipo  do campo: %c\n", p->tipoCampo);
    printf("Nome  do campo: %s\n", p->nomeCampo);
    printf("Valor do campo: %s\n", p->valorCampo);
    printf("------------------------------------\n");
  }

  printf("==============-------==============\n");
}

void dump_schema(tp_table *schema)
{
  tp_table *p;

  printf("==============SCHEMA==============\n");
  
  for (p = schema; p != NULL; p = p->next) {
    printf("------------------------------------\n");
    printf("Nome     do campo: %s\n", p->nome);
    printf("Tipo     do campo: %c\n", p->tipo);
    printf("Tamanho  do campo: %d\n", p->tam);
    printf("Chave    do campo: %d\n", p->chave);
    printf("Tabela   do campo: %s\n", p->tabelaApt);
    printf("Atributo do campo: %s\n", p->attApt);
    printf("------------------------------------\n");
  }
  
  printf("==================================\n");
}
