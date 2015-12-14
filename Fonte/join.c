#include "join.h"

tp_join *composeJoinData(qr_join *query_data, int qtt)
{
  tp_join *data = malloc(sizeof(tp_join));

  if (data == NULL) {
    fprintf(stderr, "Error while creating JOIN structures!\n");
    return data;
  }
  
  if (qtt == 0) { free(data); return NULL; }

  data->table_name    = strdup(query_data->table);	// type condition
  data->join_type     = query_data->type;
  data->current_index = 0;
  data->object        = leObjeto(data->table_name);
  data->schema        = leSchema(data->object);
  data->condition     = query_data->condition;
  data->prev          = NULL;
  data->next          = composeJoinData(++query_data, --qtt);

  if (data->next != NULL)
    data->next->prev = data;
  
  return data;
}

void resetJoinIndexes(tp_join *joins)
{
  if (joins == NULL) return;
  
  joins->current_index = 0;
  resetJoinIndexes(joins->next);
}

char *joinNext(char *tuple, int tupleSize, tp_join *join_data, tp_table *completeSchema)
{
  // Ponteiro auxiliar para iteração
  tp_join *p;

  char *newTuple = NULL  // A tupla-resultado dos JOINs
     , *subject	 = NULL; // A atual tupla de uma das tabelas do JOIN
  
  int newTupleSize = 0  // Tamanho atual da tupla-resultado
    , subjectSize  = 0  // Tamanho da atual tupla de uma das tabelas do JOIN
    , tupleIndex   = 0 // Último índice atualizado de newTuple [data]
    , i;

  // É calculado o tamanho total da tupla
  newTupleSize = schema_row_bytesize(completeSchema);
  // Então guardamos um lugar para esta
  newTuple = malloc(sizeof(char) * newTupleSize);
  // Garantímos que se algo for finalizado antes, temos um '\0' para
  // indicar o fim da tupla
  memset(newTuple, '\0', newTupleSize);

  // Copiamos os valores existentes;
  for (i = 0; i < tupleSize; i++)
    newTuple[tupleIndex++] = tuple[i];

  // Precisamos iterar uma vez em todos os JOINs para pegar o próximo
  // registro válido para comparação
  for (p = join_data; p != NULL; p = p->next) {
    // Pegamos a próxima tupla do índice do JOIN para comparar se esta
    // é válida
    subject = getTupla(p->schema, p->object, p->current_index++);

    // Então guardamos o tamanho da atual tupla; Será utilizado na
    // iteração de concatenação no futuro
    subjectSize = tamTupla(p->schema, p->object);

    // O ERRO_DE_LEITURA, neste caso, é quando a tabela chegou á seu
    // fim, nesse caso, precisamos resetar o index deste join, e
    // incremetar do join anterior.
    if (subject == ERRO_DE_LEITURA) {
      p->current_index = 0;

      if (p->prev != NULL)
	p->prev->current_index++;
      else
	return NULL; // Terminou a iteração
	
      // Como a tupla lida é inválida, precisamos reiniciar o processo
      // do JOIN com os indexes válidos
      p = join_data; // Reinicia o ponteiro *p desde o primeir JOIN

      // Reseta variaveis de controle
      free(newTuple);
      newTuple     = NULL;
      newTupleSize = 0;
    } else {
      // Sucesso, precisamos concatenar os dados da tupla encontrada
      // com os já existentes
      for (i = 0; i < subjectSize; i++)
	newTuple[tupleIndex++] = subject[i];
    }
  }

  return newTuple;
}
