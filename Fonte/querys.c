#include "buffend.h"
#include "tuple_operations.h"
#include "join.h"
#include "dump_functions.h"


int doOperation(const void * v1, const void * v2, char op, char type){ //Retorna 1 caso Verdadeiro

    if(type == 'C'){
		const char *a = *(const char **)v1;
		const char *b = *(const char **)v2;
        int num = strcmp(a, b);
        if(op == '='){
            if(num == 0)
                return 1;
            return 0;
        }
        else if(op == '!'){
            if(num != 0)
                return 1;
            return 0;
        }
    }else if(type == 'D'){
        double *a = (double *)v1;
        double *b = (double *)v2;
        if(op == '>'){
            if(*a > *b)
                return 1;
            return 0;
        }
        else if(op == '<'){
            if(*a < *b)
                return 1;
            return 0;
        }
        else if(op == '!'){
            if(*a != *b)
                return 1;
            return 0;
        }
        else if(op == '='){
            if(*a == *b)
                return 1;
            return 0;
        }
        else if(op == '@'){
            if(*a <= *b)
                return 1;
            return 0;
        }
        else if(op == '#'){
            if(*a >= *b)
                return 1;
            return 0;
        }
    }else{
        int *a =(int *)v1;
        int *b =(int *)v2;
        if(op == '>'){
            if(*a > *b)
                return 1;
            return 0;
        }
        else if(op == '<'){
            if(*a < *b)
                return 1;
            return 0;
        }
        else if(op == '!'){
            if(*a != *b)
                return 1;
            return 0;
        }
        else if(op == '='){
            if(*a == *b)
                return 1;
            return 0;
        }
        else if(op == '@'){
            if(*a <= *b)
                return 1;
            return 0;
        }
        else if(op == '#'){
            if(*a >= *b)
                return 1;
            return 0;
        }
    }
    return 2;
}


int doWhere(list_value *value){
	int b;

	//Vai parar quando não tiver mais AND ou OR. TypeLogic=N (N de NULL )
	if(value->typeLogic == 'N'){
	//if(value->next == NULL){

		if(value->typeValue == 'D')//Caso double, manda o dvalue.
			return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue);

		if(value->typeValue == 'I')//Assim para INT
			return doOperation((int*)&value->ivalue[0], (int*)&value->ivalue[1], value->typeOp, value->typeValue);
		
		if(value->typeValue == 'C')//E *char
			return doOperation((char *)&value->sname[0], (char *)&value->sname[1], value->typeOp, value->typeValue);

	}
	//Devo fazer cast.. (double *)&value->dvalue[0] .. Porque a próxima função eh constant * void.
	b = doWhere(value->next);

	if(value->typeLogic == 'A'){ //Se for AND.
		//Se b==1 devo testar o próximo AND
		if(b){ 

			if(value->typeValue == 'D')
                return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue); ;

			if(value->typeValue == 'I')
				return doOperation((int*)&value->ivalue[0], (int*)&value->ivalue[1], value->typeOp, value->typeValue);

			if(value->typeValue == 'C')
				return doOperation((char*)&value->sname[0], (char*)&value->sname[1], value->typeOp, value->typeValue);

		}
		//Caso chegue aqui, já volta 0. Porque deu False
		return 0;
	}
	if(value->typeLogic == 'O'){ //Caso seja OR
		 //Se for 0 (falso) devo testar o próximo OR
		if(b == 0){

			if(value->typeValue == 'D')
				return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue);

			if(value->typeValue == 'I')
				return doOperation((int*)&value->ivalue[0], (int*)&value->ivalue[1], value->typeOp, value->typeValue);

			if(value->typeValue == 'C')
				return doOperation((char*)&value->sname[0], (char*)&value->sname[1], value->typeOp, value->typeValue);

		}
		//Se for verdadeiro já volta 1
		return 1;
	}
	return 2;
}



column *readRegister(tp_table *campos, char **linhas, int n, int *qtdCampos){
	column *colunas;
	int i=0, sum=0,t=0,lin=0,colunasCol=0,atributos=0,count=0;
	while(i < n)
		sum += *(qtdCampos+i); //Somatorio de quantos atributos existem entre as tabelas envolvidas
	i=0;
	
	colunas = (column *)malloc(sizeof(column) * sum+1); //Tantas colunas de acordo com a quantidade de registros das tabelas
	memset(colunas, 0, sizeof(column)* n *strlen(linhas[0])); //Que??

	//t P/ Coluna DE:    **linhas[lin][t]
	//Lin P/ Linha da **linhas[lin] e campos[lin]    POIS se tratam da mesma tabela.
	//Col P/ saber quando atingiu o limite de atributos daquela tabela, hora de ler outra tabela
	//i P/ coluna[i] e campo[i], pois a ordem pode ser a mesma
	
	while(lin < n){		
		colunas[i].valorCampo = (char *)malloc(sizeof(char)*campos[i].tam+1); //Aloca o tamanho do valor de acordo com o tamanho do registro
		memset(colunas[i].valorCampo, '\0', campos[i].tam+1); //Seta em tudo '\0'.. acho que se der um erro de tamanho, já terá o \0 para ser o final.
		
		colunas[i].tipoCampo = campos[i].tipo;	//Guarda o tipo do campo.
		strcpy(colunas[i].nomeCampo, campos[i].nome); //Guarda o nome do campo.
		
		while(count < campos[i].tam){
			//Segundo setTupla, deve ser feito isso. :/
			colunas[i].valorCampo[colunasCol++] = linhas[lin][t++]; //Será? Copiando os dados
			count++;
		}
		colunas[i].valorCampo[colunasCol] = '\0';
		i++;
		atributos++;
		colunasCol=0;
		count=0;
		
		if(atributos >= *(qtdCampos+lin)){					//*(qtdCampos+x) para ter a quantidade de campos de cada Tabela, quando chegou no máximo, hora da outra tabela
			lin++;
			t=0;
		}
	}


	return colunas;
}


list_value *readyWhere(qr_select *st, column *colunas){
	
	int position=st->nfilters-1,i=0,*auxi=(int *)malloc(sizeof(int)),flag=0,full=0;
	double *auxd = (double *)malloc(sizeof(double));
	column *c = colunas; // Iterador de colunas
	list_value *value = (list_value *) malloc(sizeof(list_value));						   								//Lista de valores, usada para o WHERE
	value->next = (list_value *) malloc(sizeof(list_value));
	list_value *tmp = value;
	tmp->next = value->next;
	
	while(position >= 0){
		
		if(full == 0){																									//Full=0. Preciso primeiro preencher o atributo da esquerda.
			if(st->filters[position].left_type == 'V'){																	//Caso seja um valor
				full++;																									//Achei o valor, devo ir para o atributo da direita agora
				flag=1;																									//Flag faz a busca voltar ao primeiro att.
				if(st->filters[position].typeAtt == 'C'){																//Caso seja CHAR

					tmp->sname[0] = (char *)malloc(sizeof(char) * strlen(st->filters[position].left+1));				//Alcando o comprimento do nome +1
					strcpy(tmp->sname[0], st->filters[position].left);													//Copiando o valor
					tmp->typeValue = 'C';

				}
				else if(st->filters[position].typeAtt == 'D'){															//Caso Double
	
					tmp->dvalue[0] = atof(st->filters[position].left);													//Convertendo o valor
					tmp->typeValue = 'D';
			
				}
				else if(st->filters[position].typeAtt == 'I'){															//Caso INT
					
					tmp->ivalue[0] = atoi(st->filters[position].left);													//Conveterndo o valor
					tmp->typeValue = 'I';
				}

			}
			else if(strcmp(st->filters[position].left, c->nomeCampo) == 0){									//Se == 0. É o atributo que estou procurando.
				full++;																									//Achei o valor, devo ir para o atributo da direita agora
				flag=1;
				if(c->tipoCampo == 'C' || c->tipoCampo == 'S'){

					tmp->sname[0] = (char *)malloc(sizeof(char) * strlen(c->valorCampo)+1);						//Alcando o comprimento do valor +1
					strcpy(tmp->sname[0], c->valorCampo);
					tmp->typeValue = 'C';

				}
				else if(c->tipoCampo == 'D'){

					auxd = (double *)&c->valorCampo[0];
					tmp->dvalue[0] = *auxd;
					tmp->typeValue = 'D';
			
				}
				else if(c->tipoCampo == 'I'){															//Caso INT
					
					auxi = (int *)&c->valorCampo[0];															//Salvando o valor
					tmp->ivalue[0] = *auxi;
					tmp->typeValue = 'I';
				}
			}

		}//if( full=0 )

		else if(full == 1){																								//Salvando para o lado direito da operacao
			if(st->filters[position].right_type == 'V'){																//Caso seja um valor
				full++;																
				flag=1;
				if(st->filters[position].typeAtt == 'C'){

					tmp->sname[1] = (char *)malloc(sizeof(char) * strlen(st->filters[position].right+1));				//Alcando o comprimento do nome +1
					strcpy(tmp->sname[1], st->filters[position].right);
					tmp->typeValue = 'C';

				}
				else if(st->filters[position].typeAtt == 'D'){															//Caso Double

					tmp->dvalue[1] = atof(st->filters[position].right);
					tmp->typeValue = 'D';
			
				}
				else if(st->filters[position].typeAtt == 'I'){															//Caso Int
					
					tmp->ivalue[1] = atoi(st->filters[position].right);
					tmp->typeValue = 'I';
				}
			}
			else if(strcmp(st->filters[position].right, c->nomeCampo) == 0){									//Se == 0. É o atributo que estou procurando.
				full++;
				flag=1;																									//Encontrei os atributos, hora de setar outra lista, e voltar a buscar do 0
				if(c->tipoCampo == 'C' || c->tipoCampo == 'S'){

					tmp->sname[1] = (char *)malloc(sizeof(char) * strlen(c->valorCampo)+1);						//Alcando o comprimento do valor +1
					strcpy(tmp->sname[1], c->valorCampo);
					tmp->typeValue = 'C';

				}
				else if(c->tipoCampo == 'D'){															

					auxd = (double *)&c->valorCampo[0];															//Salvando o valor em caso de Double
					tmp->dvalue[1] = *auxd;
					tmp->typeValue = 'D';
			
				}
				else if(c->tipoCampo == 'I'){
					
					auxi = (int *)&c->valorCampo[0];															//Salvando o valor em caso de Int
					tmp->ivalue[1] = *auxi;
					tmp->typeValue = 'I';

				}
			}
		}

		if(full > 0 && flag == 1){																						//Flag serve para voltar a busca ao início

			if(full == 2){																								//Quando full=2, achei os dois operandos, hora de setar outra lista
				tmp->typeOp = st->filters[position].typeOp;

				if(position == 0)																						//Se position=0, encontrei todos os operandos, hora de fazer o WHERE
					tmp->typeLogic = st->filters[st->nfilters-1].typeLogico;

				else{																									//Ainda tenho operandos, preciso setar para a lista nova.
					tmp->typeLogic = st->filters[position-1].typeLogico;
					tmp = tmp->next;
					tmp->next = (list_value *)malloc(sizeof(list_value));
				        c = c->next;

				}
				full=0;
				c = colunas;
				//Do começo de novo
				position--;
			}
			flag=0;
			i=-1;
		}
		i++;
	}//while();
	

	return value;	
}

void startQuery(qr_select select)
{
  tp_join           *join_data;
  struct fs_objects  outerTableObject;
  tp_table          *outerTableSchema, *fullJoinSchema;
  int                i, j, tupleIsValid;
  char              *tupleData, *tupleResult;
  column            *joinColumnData, *singleColumnData;
  list_value        *temp_listvalue;
  tp_buffer         *buffer = initbuffer();

  // Verificamos se todas as tabelas existem
  // TODO: Verificar tabelas do JOIN
  if(!verificaNomeTabela(select.tables)){
    printf("\nERROR: relation \"%s\" was not found.\n\n\n", select.tables);
    return;
  }

  // Carregamos os dados da tabela principal
  outerTableObject = leObjeto(select.tables);
  outerTableSchema = leSchema(outerTableObject);

  // Verificamos se é pavê ou pacumê (ok, não é isso, mas creio que é
  // possível inferir)
  if (outerTableSchema == ERRO_DE_ALOCACAO ||
      outerTableSchema == ERRO_ABRIR_ESQUEMA) {
    fprintf(stderr, "ERROR: Could not open %s schema!\n", select.tables);
  }

  // SELECT * FROM abacate JOIN mamao ON abacate.id = mamao.id;
  if (select.njoins > 0) {
    join_data = composeJoinData(select.joins, select.njoins);

    // Itera pela tabela principal
    for (i = 0; tupleData != ERRO_DE_LEITURA; i++) {
      tupleIsValid   = 1;
      tupleData      = getTupla(outerTableSchema, outerTableObject, i);
      fullJoinSchema = createFullSchema(&select);

      while (tupleResult != NULL) {
		tupleResult = joinNext(tupleData, schema_row_bytesize(outerTableSchema), join_data, fullJoinSchema);
	// Se o resultado da tupla for NULL, chegamos ao final do
	// JOIN.
	if (tupleResult == NULL) break;

	// Ok, temos uma tupla com o próximo registro válido para testar
	// se a tupla atual satisfaz a condição de JOIN.

	// Então passamos a tupla resultante para a forma de estruturas
	// 'column', que torna mais fácil a identificação e comparação
	// de cada valor
	joinColumnData = composeTuple(tupleResult, fullJoinSchema);

	// Transformamos em uma estrutura genérica de comparação de
	// dados, como cada JOIN possui uma comparação, iteramos por
	// todos os joins.
	for (j = 0; j < select.njoins; j++) {
	  temp_listvalue = columnListValues(joinColumnData, select.joins[j].condition);

	  // Se a comparação retornou falso, a tupla já não é válida
	  if (doWhere(temp_listvalue) == 0) {
		printf("Invalido\n");
	    tupleIsValid = 0;
	    break;
	  }
	else if(select.nfilters > 0){ //Sinal que existe WHERE
		if(doWhere(readyWhere(&select, joinColumnData))){
			printf("Valido\n");
			tupleIsValid=1;
		}
		else
			printf("Invalido\n");
		}
	else //Não tem WHERE e segundo o JOIN é válida
		printf("Valido\n");
	}
	if (!tupleIsValid) {
	  free(tupleData);
	  free(fullJoinSchema);
	  free(tupleResult);
	  free(joinColumnData);
	  free(temp_listvalue);
	}	
      }
    }
  } else {
    for (i = 0; tupleData != ERRO_DE_LEITURA; i++) {
      tupleData = getTupla(outerTableSchema, outerTableObject, i);
      if (tupleData == NULL) break;
      
      singleColumnData = composeTuple(tupleData, outerTableSchema);
      temp_listvalue = readyWhere(&select, singleColumnData);

      if (doWhere(temp_listvalue) == 1) {
        storeInBuffer(tupleData, tamTupla(outerTableSchema, outerTableObject), buffer);
      }
    }
  }
}

