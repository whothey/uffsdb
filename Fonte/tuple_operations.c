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

tp_table *createSelectSchema(qr_select *select)
{
  int i;
  table    *temp_table  = iniciaTabela("temp_select");
  tp_table *temp_schema = temp_table->esquema,
           *p;

  
  if (select->projection[0][0] == '*') {
    free(temp_table);
    
    return createFullSchema(select);
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

tp_table *createFullSchema(qr_select *select)
{
  int i;
  table    *temp_table  = iniciaTabela("temp_select");
  tp_table *temp_schema = NULL,
           *p;

 
  for (p = leSchema(leObjeto(select->tables)); p != NULL; p = p->next) 
    adicionaCampo(temp_table, p->nome, p->tipo, p->tam, p->chave, p->tabelaApt, p->attApt);
  
  for (i = 0; i < select->njoins; i++) {
    for (p = leSchema(leObjeto(select->joins[i].table)); p != NULL; p = p->next)
      adicionaCampo(temp_table, p->nome, p->tipo, p->tam, p->chave, p->tabelaApt, p->attApt);
  }
  
  temp_schema = temp_table->esquema;
  
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
    c->valorCampo = malloc(sizeof(char) * (p->tam + 1));

    memset(c->valorCampo, '\0', p->tam + 1);

    fieldIterator = 0;
    while (fieldIterator < p->tam) {
     // printf("fieldIterator: %d\n", fieldIterator);
      //printf("tupleIterator: %d\n", tupleIterator);
      //printf("p->tam: %d\n", p->tam);
      
      c->valorCampo[fieldIterator] = tuple[tupleIterator];
      tupleIterator++;
      fieldIterator++;
    }
    if (p->next != NULL) {
      columns->next = malloc(sizeof(column));
      c = columns->next; 
    }
  }
  
  return columns;
}

list_value *columnListValues(column* data, qr_filter *condition)
{
  list_value *value;
  column *c = data;
  int *auxi=(int *)malloc(sizeof(int)),i=0;
  double *auxd=(double *)malloc(sizeof(double));

  value = malloc(sizeof(list_value));

  if (value == NULL) return NULL;


  while(i < 2){
		if(i == 0){
			if (condition->left_type == 'V') {	
				if(condition->typeAtt == 'C') {
					value->sname[0] = (char *)malloc(sizeof(char) * (strlen(condition->left) + 1));
					strcpy(value->sname[0], condition->left);
					value->typeValue = 'C';      
				}else if (condition->typeAtt == 'D') {
					value->dvalue[0] = atof(condition->left);
					value->typeValue = 'D';
				}else if (condition->typeAtt == 'I') {
					value->ivalue[0] = atoi(condition->left);
					value->typeValue = 'I';
				}
				i++;
			}
			else{
				if (c->tipoCampo == 'C' || c->tipoCampo == 'S') {
					value->sname[0] = (char *)malloc(sizeof(char) * strlen(c->valorCampo)+1);
					strcpy(value->sname[0], c->valorCampo);
					value->typeValue = 'C';

				}else if (c->tipoCampo == 'I') {
					auxi = (int *)&c->valorCampo[0];					
					value->ivalue[0] = *auxi;
					printf("Value0: %d\n",value->ivalue[0]);
					value->typeValue = 'I';
				
				}else if(c->tipoCampo == 'D') {
					auxd = (double *)&c->valorCampo[0];
					value->dvalue[0] = *auxd;
					value->typeValue = 'D';
				
				}	  
				i++;
				c = c->next;
			}

			}
			else if (i == 1){
			if (condition->right_type == 'V') {	
				if(condition->typeAtt == 'C') {
					value->sname[1] = (char *)malloc(sizeof(char) * (strlen(condition->right) + 1));
					strcpy(value->sname[1], condition->right);

				}else if (condition->typeAtt == 'I') {
					value->ivalue[1] = atoi(condition->right);

				}else if (condition->typeAtt == 'D') {
					value->dvalue[1] = atof(condition->right);
				}

				i++;
			}
			else{
				if (c->tipoCampo == 'C' || c->tipoCampo == 'S') {
					value->sname[1] = (char *)malloc(sizeof(char) * strlen(c->valorCampo)+1);
					strcpy(value->sname[1], c->valorCampo);

				}else if (c->tipoCampo == 'I') {
					auxi = (int *)&c->valorCampo[0];
					value->ivalue[1] = *auxi;
					printf("Value1: %d\n",value->ivalue[1]);
				
				}else if(c->tipoCampo == 'D') {
					auxd = (double *)&c->valorCampo[0];
					value->dvalue[1] = *auxd;
				
				}	  
				i++;
				c = c->next;
			}
		}
  }
	value->typeOp = condition->typeOp;
	value->typeLogic = 'N';
	value->next = NULL;


  return value;
}
