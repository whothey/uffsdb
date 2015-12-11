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
  data->next          = composeJoinData(++query_data, --qtt);
  
  return data;
}

void resetJoinIndexes(tp_join *joins)
{
  if (joins == NULL) return;
  
  joins->current_index = 0;
  resetJoinIndexes(joins->next);
}

char *joinNext(char *tuple, tp_join *join_data, tp_table *completeSchema)
{
  return NULL;
  // if (tupleIsValid())
}
