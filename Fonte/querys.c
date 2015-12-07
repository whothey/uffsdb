#include "buffend.h"

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


int selectWhere(list_value *value){
	int b;

	//Vai parar quando não tiver mais AND ou OR. TypeLogic=N (N de NULL )
	if(value->typeLogic == 'N'){

		if(value->typeValue == 'D')//Caso double, manda o dvalue.
			return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue);

		if(value->typeValue == 'I')//Assim para INT
			return doOperation((int*)&value->ivalue[0], (int*)&value->ivalue[1], value->typeOp, value->typeValue);
		
		if(value->typeValue == 'C')//E *char
			return doOperation((char *)&value->sname[0], (char *)&value->sname[1], value->typeOp, value->typeValue);

	}
	//Devo fazer cast.. (double *)&value->dvalue[0] .. Porque a próxima função eh constant * void.
	b = selectWhere(value->next);

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

void printTuplaAll(int j, column *p, int nprojection, int registros, int nvalidas){

	int i, x=0;

	if(nvalidas == 0){ //Imprimindo cabeçalho
		for(x = 0; x < nprojection; x++){

//			if(strcmp(name[i],p[x].nomeCampo) == 0){
	        if(p[x].tipoCampo == 'S')
	            printf(" %-20s ", p[x].nomeCampo);
	    	else
	            printf(" %-10s ", p[x].nomeCampo);
	        if(x < nprojection-1)
	        	printf("|");
			//i++;
			//}
		}//FOR
		printf("\n");

		for(x = 0, i = 0; i < nprojection; x++){
			//if(strcmp(name[i],p[x].nomeCampo) == 0){
				printf("%s",(p[x].tipoCampo == 'S')? "----------------------": "------------");
				if(i < nprojection-1)
					printf("+");
				i++;
			//}
		}
		printf("\n");

	}

	else if(nvalidas != 0){

		for(i = 0; i < nprojection; j++){	
			if(j == registros){
				break;

			}
			//else if(strcmp(name[i], p[j].nomeCampo) == 0){

			if(p[j].tipoCampo == 'S'){
				printf(" %-20s ", p[j].valorCampo);

			}else if(p[j].tipoCampo == 'I'){
				int *n = (int *)&p[j].valorCampo[0];
				printf(" %-10d ", *n);

			}else if(p[j].tipoCampo == 'D'){
				double *n = (double *)&p[j].valorCampo[0];
				printf(" %-10f ", *n);

			}else if(p[j].tipoCampo == 'C'){
				printf(" %-10c ", p[j].valorCampo[0]);
			}

			i++;
//			j = aux-1;
			if(i < nprojection)
				printf("|");

		}
		printf("\n");

	}




}



void printTupla(int j, column *p, char **name, int nprojection, int registros, int nvalidas, int campos){
	int i, aux=j, x=0;

	if(nvalidas == 0){ //Imprimindo cabeçalho
		if(name[0][0] == '*')
			printTuplaAll(j, p, campos, registros, nvalidas);
		else{
			for(i = 0; i < nprojection; x++){

				if(strcmp(name[i],p[x].nomeCampo) == 0){
				    if(p[x].tipoCampo == 'S')
				        printf(" %-20s ", p[x].nomeCampo);
					else
				        printf(" %-10s ", p[x].nomeCampo);
				    if(i < nprojection-1)
				    	printf("|");
					i++;
				}
			}//FOR
			printf("\n");

			for(x = 0, i = 0; i < nprojection; x++){
				if(strcmp(name[i],p[x].nomeCampo) == 0){
					printf("%s",(p[x].tipoCampo == 'S')? "----------------------": "------------");
					if(i < nprojection-1)
						printf("+");
					i++;
				}
			}
			printf("\n");
		}
	}

	if(nvalidas != 0){ //Caso tenha um registro válido irá imprimi-lo
		if(name[0][0] == '*')
			printTuplaAll(j, p, campos, registros, nvalidas);
		else{
			for(i = 0; i < nprojection; j++){	
				if(j == registros){
					break;

				}else if(strcmp(name[i], p[j].nomeCampo) == 0){

					if(p[j].tipoCampo == 'S'){
						printf(" %-20s ", p[j].valorCampo);

					}else if(p[j].tipoCampo == 'I'){
						int *n = (int *)&p[j].valorCampo[0];
						printf(" %-10d ", *n);

					}else if(p[j].tipoCampo == 'D'){
						double *n = (double *)&p[j].valorCampo[0];
					    printf(" %-10f ", *n);

					}else if(p[j].tipoCampo == 'C'){
						printf(" %-10c ", p[j].valorCampo[0]);
					}

					i++;
					j = aux-1;
					if(i < nprojection)
						printf("|");
				}
			}
			printf("\n");
		}
	}
}



void doSelect(qr_select *st){

	int j=0,erro,x,p,position=0,flag=1,primeiraTupla=1,m=0,n=0,registros,nvalidas=0,aux=0,full=0;
	int *poAtt, *auxi=(int *)malloc(sizeof(int));
	double *auxd = (double *)malloc(sizeof(double));									   //Vetores auxiliares para conversão do valor.

	struct fs_objects objeto = leObjeto(st->tables);

	list_value *value = (list_value *) malloc(sizeof(list_value));						   //Lista de valores, usada no SELECT
	value->next = (list_value *) malloc(sizeof(list_value));
	list_value *tmp = value;
	tmp->next = value->next;

    if(!verificaNomeTabela(st->tables)){ 												   //Verificação nome da Tabela
        printf("\nERROR: relation \"%s\" was not found.\n\n\n", st->tables);
        return;
    }
 
    tp_table *esquema = leSchema(objeto);
    if(esquema == ERRO_ABRIR_ESQUEMA){													   //Se conseguiu criar o esquema?
        printf("ERROR: schema cannot be created.\n");
        free(esquema);
        return;
    }

    tp_buffer *bufferpoll = initbuffer();												   //Bufferpool
    if(bufferpoll == ERRO_DE_ALOCACAO){
        free(bufferpoll);
        free(esquema);
        printf("ERROR: no memory available to allocate buffer.\n");
        return;
    }	
																						   //'Vetor' que vai guardar todos os acessos a pagina. Para acessar de maneira direta.
	poAtt = (int *)malloc(sizeof(int) *( objeto.qtdCampos * 2 )); 						   //No máximo terá o tamanho igual a quantidade de campos.		   
	erro = SUCCESS;																		   //Coloca todas as tuplas daquela tabela no buffer
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);

	column *pagina;
	tmp->sname[0] = (char *)malloc(esquema->tam * 10);									   //Alocando o espaço de acordo com o tamanho daquela tabela
	tmp->sname[1] = (char *)malloc(esquema->tam * 10);									   
	int ntuplas=--x;
	p = 0 * ntuplas;

	while(x){																			   //Vou começar a ler e salvar na struct *value
																						   //Lê uma página p do buffer e salva na variavel *pagina
		pagina = getPage(bufferpoll, esquema, objeto, p);
	    if(pagina == ERRO_PARAMETRO){
            printf("ERROR: could not open the table.\n");
            free(bufferpoll);
            free(esquema);
            return;
	    }
		registros = objeto.qtdCampos * bufferpoll[p].nrec;								   //Quantidade de campos * número de registros(linhas).		

		if(primeiraTupla){																   //Quando for a 1° tupla, vou varrer a pagina, encontrar as posições certas e salvar no *poAtt.
			printTupla(0,pagina,st->projection,st->nprojection,registros,0,objeto.qtdCampos);			   //Imprimir o cabeçalho!

			for(j=0; st->nfilters > position; j++){										   //Número de filtros maior que a posição atual do filtro
				if(full == 0){															   //Verificar se já achou o atributo da esquerda, caso full=1 eh porque já achou.
					if(st->filters[position].left_type == 'V'){							   //Se lado esquero não for um atributo, mas sim direto o valor
						*(poAtt + n) = -1; 												   //Quando for -1 eh porque o valor vem direto do filters.left
						n++;
						full=1;
						if(st->filters[position].typeAtt == 'C'){			   			   //Caso o tipo seja Char
							strcpy(tmp->sname[0], st->filters[position].left); 			   //Copio direito do filtro
							tmp->typeValue = 'C'; 							   			   //typeValue=C porque eh CHAR

						}else if(st->filters[position].typeAtt == 'D'){ 		  		   //Caso seja Double
							tmp->dvalue[0] = atof(st->filters[position].left); 			   //Converto direito do filtro
							tmp->typeValue = 'D';							   			   //typeValue=D porque eh DOUBLE

						}else if(st->filters[position].typeAtt == 'I'){		   			   //Caso seja Int
							tmp->ivalue[0] = atoi(st->filters[position].left); 			   //Converto direto do filtro
							tmp->typeValue = 'I'; 							   			   //typeValue=I porque eh INT
						}
																						   //Se não for valor, então eh uma coluna. Vou buscar o valor dela na *pagina
					}else if(strcmp(pagina[j].nomeCampo, st->filters[position].left) == 0){//Se o nome do campo e o nome do filtro são iguais retorna 0
						*(poAtt+n) = j+objeto.qtdCampos; 								   //Recebe a próxima ocorrência desse campo, para acessar a próxima tupla de maneira direta.s
						n++;
						full=1;															   //Sempre que for full=1 é porque encontrei o valor da esquerda daquela página
																  						   //Agora salvar na lista Value.
						if(pagina[j].tipoCampo == 'S' || pagina[j].tipoCampo == 'C'){	   //Caso seja CHAR ou STRING
							strcpy(tmp->sname[0],pagina[j].valorCampo);				   	   //Copio o conteúdo da pagina.
							tmp->typeValue = 'C';									   	   //typeValue=C porqueh eh CHAR

						}else if(pagina[j].tipoCampo == 'D'){
							auxd = (double *)&pagina[j].valorCampo[0];				  	   //Salvando o conteúdo da página para DOUBLE	
							tmp->dvalue[0] = *auxd;
							tmp->typeValue = 'D';

						}else if(pagina[j].tipoCampo == 'I'){							   //Salvando o conteúdo da página para INT
							auxi = (int *)&pagina[j].valorCampo[0];
							tmp->ivalue[0] = *auxi;
							tmp->typeValue = 'I';
						}
					}
				}else if(full == 1){														   //full=1 sinal que já encontrou o att da esquerda. Antes disso não pode encontrar o da direita
					flag=0;
					if(st->filters[position].right_type == 'V'){						   //Caso o lado direito não fosse um atributo, mas sim um valor
						*(poAtt+n) = -2; 												   //Quando for -2 eh porque o valor vem direto do filters.right (da consulta)
						n++;
						full = 2;														   //Achou o att da direita
						flag=1;

						if(st->filters[position].typeAtt == 'C'){						   //Caso seja CHAR ou STRING
							st->filters[position].right[strlen(st->filters[position].right)] = '\0';
							strcpy(tmp->sname[1], st->filters[position].right);
							tmp->typeValue = 'C';

						}else if(st->filters[position].typeAtt == 'D'){					   //Caso seja DOUBLE
							tmp->dvalue[1] = atof(st->filters[position].right);
							tmp->typeValue = 'D';

						}else if(st->filters[position].typeAtt == 'I'){ 				   //Caso seja INT
							tmp->ivalue[1] = atoi(st->filters[position].right);
							tmp->typeValue = 'I';
						}

					}else if(strcmp(pagina[j].nomeCampo, st->filters[position].right) == 0){ //Caso seja uma coluna, preciso buscar o valor na página
						*(poAtt+n) = j+objeto.qtdCampos;								   //Recebe a ocorrência desse próximo atributo, para depois acessá-lo de maneira direta
						n++;
						full=2;															   //Encontrei o atributo da direita.
						flag=1;															   //Para controlei. Preciso setar uma nova lista *value
																						   //Agora salvar na lista Value.
						if(pagina[j].tipoCampo == 'S' || pagina[j].tipoCampo == 'C'){      //Caso seja STRING ou CHAR
							strcpy(tmp->sname[1], pagina[j].valorCampo);
							tmp->typeValue = 'C';

						}else if(pagina[j].tipoCampo == 'D'){							   //Caso seja DOUBLE
							auxd = (double *)&pagina[j].valorCampo[0];
							tmp->dvalue[1] = *auxd;
							tmp->typeValue = 'D';

						}else if(pagina[j].tipoCampo == 'I'){							   //Caso seja INT
							auxi = (int *)&pagina[j].valorCampo[0];
							tmp->ivalue[1] = *auxi;
							tmp->typeValue = 'I';
						}
				
					}
				}
				
				if(full > 0 && flag == 1){												   //Se verdadeiro. Preciso começar a página novamente.
					if(full == 2){														   //Quando full=2, é sinal que preciso criar novo value, e setar o próximo filtro. Pois tudo já foi preenchido
						aux = j;													       //Preciso salvar o valor do J(controle da pagina). Para no próximo passo.
						tmp->typeLogic = st->filters[position].typeLogico;				   //Preenchendo a lista value com o tipoLogico, A ou O. 
						tmp->typeOp = st->filters[position].typeOp;						   //Preenchendo a operação '<', '!=' .....
						full = 0;														   //Voltar a preencher o novo value
						tmp = tmp->next;												   //Novo value
						tmp->next =  (list_value *) malloc(sizeof(list_value)); 	  	   //próxima lista de values
						tmp->sname[0] = (char *)malloc(esquema->tam * 10);			   	   //Alocando o espaço correto para os nomes 
						tmp->sname[1] = (char *)malloc(esquema->tam * 10);
						position++; 													   //Próximo filtro
					}
					j=-1;																   //Preciso voltar a verificar os registros a partir do primeiro. Método SCAN
				}
			} 																			   //Fim do FOR
																						   //Acabei de ler o primeiro registro.. primeira tupla
			primeiraTupla=0;
			if(selectWhere(value)){														   //Caso seja um registro válido retorna 1. Caso não retorna 0
				nvalidas++;																   //Quantia de linhas válidas
				printTupla(0,pagina,st->projection,st->nprojection,registros,nvalidas,objeto.qtdCampos);	   //Imprimindo a tupla
			}
		}																				   //Fim IF da primeira tupla
/********************************************************************************************************************|
| Até aqui fiz apenas pro primeiro registro. E claro, salvei a posição dos próximos atributos no 'vetor' poAtt. 	 |
| Agora eh só pegar o resto dos registros, acessá-los diretamente. Salvar no *value, jogar no select e imprimir.	 |
|********************************************************************************************************************/
		if(primeiraTupla == 0){															   //Se não for primeira tupla vem aqui.
			tmp = value;																   //Preciso voltar a apontar pro inicio do da Lista Value
			tmp->next = value->next;
			position = 0;																   //Ler o filtro do começo
			j=1;
			while(*(poAtt+m) < registros){												   //*(poAtt+m) irá guardar o valor do próximo registro
				
				if(st->filters[position].typeAtt == 'C'){								   //Caso CHAR
						if(*(poAtt+m) == -1) 											   //Caso for -1, valor vem direto do operador ESQUERDO no where
							strcpy(tmp->sname[0], st->filters[position].left);
						else{															   //Se não, ele vem da página
							strcpy(tmp->sname[0], pagina[*(poAtt+m)].valorCampo);
							*(poAtt+m) += objeto.qtdCampos;								   //A posicao do próximo registro
						}
						full = 1;														   //Já achei o atributo da esquerda
					if(*(poAtt+m+1) < registros){										   //Se ainda for uma posicao válida da pagina
						if(*(poAtt+m+1) == -2)											   //Caso seja -2, atributo vem direto do operador DIREITO no where
							strcpy(tmp->sname[1], st->filters[position].right);
						else{															   //Se não, vem da página
							strcpy(tmp->sname[1], pagina[*(poAtt+m+1)].valorCampo);
							*(poAtt+m+1) += objeto.qtdCampos;
						}
						full=2;															   //Achei o segundo atributo, da direita.
					}

				}else if(st->filters[position].typeAtt == 'D'){							   //Caso DOUBLE
						if(*(poAtt+m) == -1)
							tmp->dvalue[0] = atof(st->filters[position].left);			   //Converter STRING em DOUBLE
						else{															   //Valor retirado da página
							auxd = (double *)&pagina[*(poAtt+m)].valorCampo[0];			
							tmp->dvalue[0] = *auxd;
							*(poAtt+m) += objeto.qtdCampos;								   //Próxima ocorrência desse atributo
						}
						full = 1;
					if(*(poAtt+m+1) < registros){
						if(*(poAtt+m+1) == -2)											   //Caso DOUBLE para o atributo da DIREITA
							tmp->dvalue[1] = atof(st->filters[position].right);
						else{
							auxd = (double *)&pagina[*(poAtt+m+1)].valorCampo[0];
							tmp->dvalue[1] = *auxd;
							*(poAtt+m+1) += objeto.qtdCampos;
						}
						full=2;
					}

				}else if(st->filters[position].typeAtt == 'I'){							   //Caso INT
					if(*(poAtt+m) == -1)
							tmp->ivalue[0] = atoi(st->filters[position].left);
						else{
							auxi = (int *)&pagina[*(poAtt+m)].valorCampo[0]; 
							tmp->ivalue[0] = *auxi;
							*(poAtt+m) += objeto.qtdCampos;
						}
						full = 1;
					if(*(poAtt+m+1) < registros){
						if(*(poAtt+m+1) == -2){
							tmp->ivalue[1] = atoi(st->filters[position].right);			   //Caso INT, atributo vem direto do filtro.
						}
						else{
							auxi = (int *)&pagina[*(poAtt+m+1)].valorCampo[0];
							tmp->ivalue[1] = *auxi;
							*(poAtt+m+1) += objeto.qtdCampos;	
						}
						full=2;
					}
				}

				if(full == 2){ 														      //Encheu a lista, setar para a  nova
					tmp->typeLogic = st->filters[position].typeLogico;
					tmp->typeOp = st->filters[position].typeOp;
					tmp->typeValue = st->filters[position].typeAtt;					      //Salvando o tipo: Logico, Operacao, Atributo
					position++;														      //Próximo filtro
					m += 2;															      //Próximos valores
					full=0;	
					tmp = tmp->next; 												      //Proxima lista
					if(m == n){														      //Se verdadeiro, terminou um registro
						aux += objeto.qtdCampos;									      //Variavel para impressão, sabe extamente o início desse tupla
						j++;
						if(selectWhere(value)){
							nvalidas++;													  //Contador de Tuplas imprimidas
							printTupla(aux,pagina,st->projection,st->nprojection,registros,nvalidas,objeto.qtdCampos);//Bora imprimir
						}
						m=0;														      //Para voltar a armazenar do 0.
						tmp = value;
						tmp->next = value->next;
						position=0;		
					}
				}
			}//while(*(poAtt+m) < registros)
			int abacate;
			if(p==0)
				j++;
			for(abacate=0; abacate < n; abacate++){ //Setando para o vetor para o início.2
				if(*(poAtt+abacate) > -1)
					*(poAtt+abacate) = *(poAtt+abacate) - objeto.qtdCampos * (j-1);
			}
			aux=-objeto.qtdCampos;
		}//IF primeira==0;														  	      //Irá redefinir as posições dos atributos

	x -= bufferpoll[p++].nrec;														      //Quantidade de Tuplas lidas - quantidade armazenada nesta página
	}//While(x)
	printf("(%d rows)\n\n",nvalidas);

}





























