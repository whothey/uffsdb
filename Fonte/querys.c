#include "buffend.h"
#include "tuple_operations.h"
#include "join.h"


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

/*readRegister();
Objetivo: Teoricamete irá ler o tripão, **linhas, e salvará os valores contidos lá
numa variavel column, para então ler ela e preencher o list_value para fazer o WHERE.
Retorno :	Column *Colunas	com todos os registros
*colunas:	Colunas que conterão os valores
**linhas:	A sequência de Char's contendo todos os bytes.
n		:	Contador de quantas linhas existem. Tabelas envolvidas
*campos : 	'Vetor' dos atributos das tabelas envolvidas, pois preciso do tamanho,nome, tipo do campo de cada tabela
			Linha para cada Tabelas
*qtdCampos:	'Vetor' que contém a quantidade de campos de cada tabela envolvida.

double *d = (double *)&colunas[i].valorCampo[0];				  	   //Salvando o conteúdo da Coluna em DOUBLE
int *i= (int *)&colunas[i].valorCampo[0];							  //Salvando o conteúdo da Coluna em INT


*/

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




int newDoWhere(qr_select *st, tp_table *campos){
	
	int ready=1; //position=st->nfilters-1;
	//column *colunas = readRegister(campos, linhas, n, qtdCampos);													//Lendo os registros e salvando em *tuplas.											
	
	list_value *value = (list_value *) malloc(sizeof(list_value));						   //Lista de valores, usada no SELECT
	value->next = (list_value *) malloc(sizeof(list_value));
	list_value *tmp = value;
	tmp->next = value->next;
	//tmp->sname[0] = (char *)malloc(sizeof(char) * campos[n-1].tam * 10);									   //Alocando o espaço de acordo com o tamanho daquela tabela
	//tmp->sname[1] = (char *)malloc(campos[n-1].tam * 10);
	
	
	while(ready){
		
		
		
		
		
		
		
		
		
	}
	
		return 1;
	
}











	
	
	
	
/*	


void doSelect(qr_select *st){
	int x, erro;	
	struct fs_objects objeto = leObjeto(st->tables);

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
	erro = SUCCESS;																		   //Coloca todas as tuplas daquela tabela no buffer
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);
   
	int ntuplas = --x;
	int op = whatOp(st);
	switch(op){
		//Caso sem JOIN sem WHERE. Ou seja, só imprimir.
		case 1:
			noJoinNoWhere(st, bufferpoll, &objeto, esquema, ntuplas);
			break;
		//Caso sem JOIN com WHERE
		case 2:
			int j=0,position=0,flag=1,primeiraTupla=1,m=0,n=0,registros,nvalidas=0,aux=0,full=0;
			int *poAtt, *auxi=(int *)malloc(sizeof(int));
			double *auxd = (double *)malloc(sizeof(double));									   //Vetores auxiliares para conversão do valor.
			list_value *value = (list_value *) malloc(sizeof(list_value));						   //Lista de valores, usada no SELECT
			value->next = (list_value *) malloc(sizeof(list_value));
			list_value *tmp = value;
			tmp->next = value->next;
			tmp->sname[0] = (char *)malloc(esquema->tam * 10);									   //Alocando o espaço de acordo com o tamanho daquela tabela
			tmp->sname[1] = (char *)malloc(esquema->tam * 10);
			//'Vetor' que vai guardar todos os acessos a pagina. Para acessar de maneira direta.
			poAtt = (int *)malloc(sizeof(int) *( objeto.qtdCampos * 2 )); 						   //No máximo terá o tamanho igual a quantidade de campos.		   
		
		
			break;
		//Caso com JOIN sem WHERE
		case 3:
		
			break;
		//Caso com JOIN com WHERE
		case 4:
		
			break;
		
		
		
	}
}






 whatOp();
Objetivo: Retornar o valor para cada operação.
	1 - Caso seja sem JOIN sem WHERE
	2 - Caso seja sem JOIN com WHERE
	3 - Caso seja com JOIN sem WHERE
	4 - Caso seja com JOIN com WHERE
int whatOp(qr_select *st){

	if(st->nfilters == 0){ //sem WHERE
		if(st->njoins == 0) //sem JOIN
			return 1; //sem JOIN sem WHERE
		return 3; //com JOIN sem WHERE
	}
	if(st->njoins == 0) //sem JOIN
		return 2; //sem JOIN com WHERE
	return 4; //com JOIN com WHERE
}
void printCabecalho2(column *p, int x){
	
	if(p[x].tipoCampo == 'S')
		printf(" %-20s ", p[x].nomeCampo);
	else
		printf(" %-10s ", p[x].nomeCampo);
	
}

void printCabecalho(column *p, char **name, int nprojection, int j){
	int i=0,aux=j;
	
	if(name[0][0] == '*'){
		for(i=0; i < nprojection; j++,i++){
			printCabecalho2(p, j);
			if(i < (nprojection-1))
				printf("|");
		}
		printf("\n");
		
		for(j=aux, i=0; i < nprojection; j++,i++){
			printf("%s",(p[j].tipoCampo == 'S')? "----------------------": "------------");
			if(i < nprojection-1)
				printf("+");
		}
		printf("\n");
		return;
	}
	
	while(i < nprojection){
		if(strcmp(name[i],p[j].nomeCampo) == 0){
			printCabecalho2(p, j);
			if(i < (nprojection-1))
				printf("|");
			i++;
		}
		j++;
	}
	printf("\n");
	
	for(j=aux,i=0; i < nprojection; j++){
		if(strcmp(name[i],p[j].nomeCampo) == 0){
			printf("%s",(p[j].tipoCampo == 'S')? "----------------------": "------------");
			if(i < nprojection-1)
				printf("+");
			i++;
		}
	}
	printf("\n");
}



void printTupla2(column *p, int j){
	
	if(p[j].tipoCampo == 'S'){
		printf(" %-20s ", p[j].valorCampo);

	}else if(p[j].tipoCampo == 'I'){
		int *n = (int *)&p[j].valorCampo[0];
		printf(" %-10d ", *n);

	}else if(p[j].tipoCampo == 'D'){
		double *n = (double *)&p[j].valorCampo[0];
		printf(" %-10f ", *n);

	}else if(p[j].tipoCampo == 'C')
		printf(" %-10c ", p[j].valorCampo[0]);
}


int naoImprimi(int *v, int i, int j){
	int x;
	for(x=0; x < i; x++)
		if(*(v + x) == j)
			return 0;
	return 1;
}


void printTupla(int j, column *p, char **name, int nprojection, int join){
	int i, aux=j, v[nprojection];
	
	if(join){//Precisa ser implementado
		
		
		return;
	}
	
	else{ //Quando nao tiver JOIN imprime aqui
		if(name[0][0] == '*'){//Imprimindo quando for *
			for(i = 0; i < nprojection; j++,i++){
				printTupla2(p, j);
				if(i < (nprojection-1))
					printf("|");
			}
			printf("\n");
			
			return;
		}
			
		for(i = 0; i < nprojection; j++){//Imprimindo quando não for *
			if(strcmp(name[i], p[j].nomeCampo) == 0){
				if(naoImprimi(v, i, j)){//Retorna 0 se já IMPRIMI, caso não RETORNA 1
					printTupla2(p, j);
					v[i++] = j;
					j = aux;
					if(i < nprojection)
						printf("|");
				}
			}
		}
		printf("\n");
	}
}

void noJoinNoWhere(qr_select *st, tp_buffer *bufferpoll, struct fs_objects *objeto, tp_table *esquema, int registros){
	int x,p=0,i=0, nvalidas=0;	
	column *pagina;

	if(st->projection[0][0] == '*')
		st->nprojection = objeto->qtdCampos;
	while(registros){
		pagina = getPage(bufferpoll, esquema, *objeto, p);
	    if(pagina == ERRO_PARAMETRO){
            printf("ERROR: could not open the table.\n");
            free(bufferpoll);
            free(esquema);
            return;
	    }
		if(p == 0)
			printCabecalho(pagina, st->projection, st->nprojection, 0);
		x = objeto->qtdCampos * bufferpoll[p].nrec;								   //Quantidade de campos * número de registros(linhas).
		while(i < x){
			printTupla(i, pagina, st->projection, st->nprojection, 0);
			i += objeto->qtdCampos;
			nvalidas++;
		}		
		i=0;
		registros -= bufferpoll[p++].nrec;										  //Quantidade de Tuplas lidas - quantidade armazenada nesta página
	}
	printf("(%d rows)\n\n",nvalidas);
	
}









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
********************************************************************************************************************|
| Até aqui fiz apenas pro primeiro registro. E claro, salvei a posição dos próximos atributos no 'vetor' poAtt. 	 |
| Agora eh só pegar o resto dos registros, acessá-los diretamente. Salvar no *value, jogar no select e imprimir.	 |
|********************************************************************************************************************
		if(primeiraTupla == 0){															   //Se não for primeira tupla vem aqui.
			tmp = value;																   //Preciso voltar a apontar pro inicio do da Lista Value
			tmp->next = value->next;
			position = 0;																   //Ler o filtro do começo
			j=1;
			while(*(poAtt+m) < registros){												   //(poAtt+m) irá guardar o valor do próximo registro
				
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

*/



























