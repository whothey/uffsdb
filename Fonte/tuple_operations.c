#include "tuple_operations.h"

int schema_column_count(tp_table *schema)
{
  int i;
  tp_table *p; // Pointer iterator
  
  for (i = 0, p = schema; p != NULL; p = p->next, i++);

  return i;
}

int schema_row_bytesize(tp_table *schema)
{
  int size = 0;
  tp_table *p;

  for (p = schema; p != NULL; p = p->next)
    size += p->tam;

  return size;
}

tp_table *createFullSchema(qr_select *select)
{
  int i;
  table    *temp_table  = iniciaTabela("temp_select");
  tp_table *temp_schema = temp_table->esquema,
           *p;

  
  if (select->projection[0][0] == '*') {
    for (p = leSchema(leObjeto(select->tables)); p != NULL; p = p->next)
      adicionaCampo(temp_table, p->nome, p->tipo, p->tam, p->chave, p->tabelaApt, p->attApt);

    for (i = 0; i < select->njoins; i++) {
      for (p = leSchema(leObjeto(select->joins[i].table)); p != NULL; p = p->next)
	adicionaCampo(temp_table, p->nome, p->tipo, p->tam, p->chave, p->tabelaApt, p->attApt);
    }
  } else {
    for (p = leSchema(leObjeto(select->tables)); p != NULL; p = p->next) {
      for (i = 0; i < select->nprojection; i++) {
	if (strcmp(p->nome, select->projection[i]))
	  adicionaCampo(temp_table, p->nome, p->tipo, p->tam, p->chave, p->tabelaApt, p->attApt);
      }
    }

    for (i = 0; i < select->njoins; i++) {
      for (p = leSchema(leObjeto(select->joins[i].table)); p != NULL; p = p->next) {
	if (strcmp(p->nome, select->projection[i]))
	  adicionaCampo(temp_table, p->nome, p->tipo, p->tam, p->chave, p->tabelaApt, p->attApt);
      }
    }
  }

  free(temp_table);

  return temp_schema;
}

column *composeTuple(char *tuple, tp_table *schema)
{
  column *columns = (column *)malloc(sizeof(column)),
         *c = NULL; // Current Column
  tp_table *p;
  int fieldIterator = 0, tupleIterator = 0;

  if(!columns)
    return ERRO_DE_ALOCACAO;

  for (p = schema, c = columns; p != NULL; p = p->next) {
    c->tipoCampo  = p->tipo;
    strcpy(c->nomeCampo, p->nome);
    c->valorCampo = malloc(sizeof(char) * p->tam);

    memset(c->valorCampo, '\0', p->tam);

    fieldIterator = 0;
    while (fieldIterator <= p->tam)
      c->valorCampo[fieldIterator++] = tuple[tupleIterator++];

    if (p->next != NULL) {
      columns->next = malloc(sizeof(column));
      c = columns->next; 
    }
  }
    
  return columns;
}
