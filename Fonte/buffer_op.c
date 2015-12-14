//BufferPool
#include "buffend.h"
#include "tuple_operations.h"

// RETORNA PAGINA DO BUFFER
column *getBufferPage(tp_buffer *buffer, tp_table *schema, int page)
{
  int column_count = schema_column_count(schema);
  
  if(page >= PAGES)
    return ERRO_PAGINA_INVALIDA;

  if(buffer[page].nrec == 0) //Essa página não possui registros
    return ERRO_PARAMETRO;

  column *colunas = (column *)malloc(sizeof(column) * column_count * buffer[page].nrec); //Aloca a quantidade de campos necessária

  if(!colunas)
    return ERRO_DE_ALOCACAO;

  memset(colunas, 0, sizeof(column) * column_count * buffer[page].nrec);

  int i=0, j=0, t=0, h=0;

  if (!buffer[page].position)
    return colunas;

  while(i < buffer[page].position){
    t=0;
    if(j >= column_count)
      j=0;

    colunas[h].valorCampo = (char *)malloc(sizeof(char)*schema[j].tam+1);
    memset(colunas[h].valorCampo, '\0', schema[j].tam+1);
    colunas[h].tipoCampo = schema[j].tipo;  //Guarda tipo do campo

    strcpy(colunas[h].nomeCampo, schema[j].nome); //Guarda nome do campo

    while(t < schema[j].tam){
      colunas[h].valorCampo[t] = buffer[page].data[i]; //Copia os dados
      t++;
      i++;
    }
    colunas[h].valorCampo[t] = '\0';

    h++;
    j++;
  }

  return colunas; //Retorna a 'page' do buffer
}
