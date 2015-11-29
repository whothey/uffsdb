#include "buffend.h"


void printCabecalho(column *p, char **name, int nprojection){
	int j=0, i;
	for(i = 0; i < nprojection; j++){

		if(strcmp(name[i],p[j].nomeCampo) == 0){
            if(p[j].tipoCampo == 'S')
                printf(" %-20s ", p[j].nomeCampo);
        	else
                printf(" %-10s ", p[j].nomeCampo);
            if(i < nprojection-1)
            	printf("|");
			i++;
		}
	}//FOR
	printf("\n");

    for(j = 0, i = 0; i < nprojection; j++){
		if(strcmp(name[i],p[j].nomeCampo) == 0){
		    printf("%s",(p[j].tipoCampo == 'S')? "----------------------": "------------");
		    if(i < nprojection-1)
		    	printf("+");
			i++;
		}
	}
    printf("\n");
}

void printSaveTupla(int j, column *p, char **name, int nprojection, int registros){
	int i;
	
	for(i = 0; i < nprojection; j++){
		
		if(j < registros){
			printf("Nome: %s\n",p[j].nomeCampo);
			if(strcmp(name[i], p[j].nomeCampo) == 0){

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
				printf("|");
			}
			
		}else
			i = nprojection;
	}

}

void printTupla(int j, column *p, char **name, int nprojection, int registros){
	int i, aux=j;

	for(i = 0; i < nprojection; j++){
		
		if(j == registros){
			//printf("break\n");
			break;
		}
		//printf("\nName[%d]: %s\np[%d]: %s\n",i,name[i],j,p[j].nomeCampo);

		if(strcmp(name[i], p[j].nomeCampo) == 0){

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



void doSelect(qr_select *st){

	int j=0,erro,x,p,position=0,flag=1,primeiraTupla=1,valido=-10,m=0,n=0,count=0,registros,i,nvalidas=0,save=0;
//aux=j
	int *poAtt;
	double *auxd = (double *)malloc(sizeof(double));
	int *auxi = (int *)malloc(sizeof(int));

	struct fs_objects objeto = leObjeto(st->tables);

	list_value *value = (list_value *) malloc(sizeof(list_value));
	value->next = (list_value *) malloc(sizeof(list_value));

    if(!verificaNomeTabela(st->tables)){ //Verificação nome da Tabela
        printf("\nERROR: relation \"%s\" was not found.\n\n\n", st->tables);
        return;
    }
 
    tp_table *esquema = leSchema(objeto);
    if(esquema == ERRO_ABRIR_ESQUEMA){
        printf("ERROR: schema cannot be created.\n");
        free(esquema);
        return;
    }
	//Bufferpool
    tp_buffer *bufferpoll = initbuffer();
    if(bufferpoll == ERRO_DE_ALOCACAO){
        free(bufferpoll);
        free(esquema);
        printf("ERROR: no memory available to allocate buffer.\n");
        return;
    }
	//'Vetor' que vai guardar todos os acessos a pagina. Para pegar os atributos de maneira direta
	poAtt = (int *)malloc(sizeof(int) *( objeto.qtdCampos * 2 )); //No máximo terá o tamanho igual a quantidade de campos.
	//poProj = (int *)malloc(sizeof(int) * st->nprojection);
	//int * positions = (int *)malloc(sizeof(int)*st->nprojection);

	//Coloca todas as tuplas daquela tabela no buffer
	erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);

	list_value *tmp = value;
	tmp->next = value->next;
	int ntuplas=--x,full=0;
	//int full=0;
	p = 0 * ntuplas;
	
	//Vou começar a ler e salvar na struct *value
	column *auxpag;
	column *pagina;
	while(x){

		if(save){
			i = j;
			auxpag = pagina;
		}
		//Lê uma página p do buffer e salva na variavel *pagina
		pagina = getPage(bufferpoll, esquema, objeto, p);
	    if(pagina == ERRO_PARAMETRO){
            printf("ERROR: could not open the table.\n");
            free(bufferpoll);
            free(esquema);
            return;
	    }
		count=0;

	/*	for(i=0; i < st->nprojection; i++){
			if(strcmp(st->projection[i], pagina[j].nomeCampo) == 0){
				*(poProj + i) = j;
				*(positions + i) = j;
				i++;
			}
			j++;
		}
		Salvando as posições para impressão.. não eh necessario*/

		registros = objeto.qtdCampos * bufferpoll[p].nrec;
		
		if(primeiraTupla){
		//Quando for a 1° tupla, vou varrer todo o objeto, encontrar as posições certinhas, salvar no *poAtt. E na próxima página acessar tudo tipo ninja(de maneira direta)
			for(j=0; st->nfilters > position; j++){
				//Verificar se já achou o atributo da esquerda, caso full=1 eh porque já achou.
				//Mais rápido perguntar se full==0 do que perguntar a strcmp se pagi..... são iguais
				if(full == 0){
					//Se lado esquero não for um atributo, mas sim direto o valor
					if(st->filters[position].left_type == 'V'){
						*(poAtt + n) = -1; //Quando for -1 eh porque o valor vem direto do filters.left
						n++;
						full=1;
						//Caso o tipo seja Char
						if(st->filters[position].typeAtt == 'C'){
							if(valido == -10)
								tmp->sname[0] = (char *)malloc(sizeof(char)*esquema->tam + sizeof(char)*2);//Alocando de acordo com o tamanho do campo +1
							strcpy(tmp->sname[0], st->filters[position].left); //Copio direito do filtro
							tmp->typeValue = 'C'; 
						}
						//Caso seja Double
						else if(st->filters[position].typeAtt == 'D'){
							tmp->dvalue[0] = atof(st->filters[position].left); //Converto direito do filtro
							tmp->typeValue = 'D';
						}
						//Caso seja Int
						else if(st->filters[position].typeAtt == 'I'){
							tmp->ivalue[0] = atoi(st->filters[position].left); //Converto direto do filtro
							tmp->typeValue = 'I'; //typeValue=I porque eh INT
						}
					}

					//Se não for valor, então eh uma coluna. Então preciso buscar o valor dela na *pagina
					else if(strcmp(pagina[j].nomeCampo, st->filters[position].left) == 0){ //Se o nome do campo e o nome do filtro são iguais volta 0
						//N eh pra controlar o 'vetor'22 poAtt
						*(poAtt+n) = j+objeto.qtdCampos; //Recebe a próxima ocorrência desse campo, para pegar o valor de maneira direta
						n++;
						full=1;//Sempre que for full=1 é porque encontrei o valor da esquerda daquela página
						//Agora salvar na lista Value.
						if(pagina[j].tipoCampo == 'S' || pagina[j].tipoCampo == 'C'){
							if(valido == -10){
								tmp->sname[0] = (char *)malloc(sizeof(char)*strlen(pagina[j].valorCampo) * 2);
							}
							//printf("NomeCampo: %s\n" , pagina[j].nomeCampo);
							strcpy(tmp->sname[0],pagina[j].valorCampo);
							//printf("Nome1: %s\n", tmp->sname[0]);
							tmp->typeValue = 'C';
							//printf("valor: %d\n", *(poAtt+n-1));
						}

						else if(pagina[j].tipoCampo == 'D'){
							auxd = (double *)&pagina[j].valorCampo[0];
							tmp->dvalue[0] = *auxd;
							tmp->typeValue = 'D';
						}

						else if(pagina[j].tipoCampo == 'I'){
							auxi = (int *)&pagina[j].valorCampo[0];
							tmp->ivalue[0] = *auxi;
							//printf("NomeCampoI: %s\n" , pagina[j].nomeCampo);
							//printf("NomeI: %d\nj: %d\n\n", *auxi, j);
							tmp->typeValue = 'I';
							// O que acontece eh que ele volta aqui, porque as duas vezes eh o att idade.
						}
					}
				}

				//full = 1 sinal que já encontrou o att da esquerda. Antes disso não pode achar o da direita
				else if(full == 1){
					flag=0;
					//Se o lado direito não for um atributo, mas sim um valor
					if(st->filters[position].right_type == 'V'){
						*(poAtt+n) = -2; //Quando for -2 eh porque o valor vem direto do filters.right
						n++;
						full = 2;
						flag=1;
						if(st->filters[position].typeAtt == 'C'){
							if(valido == -10){
								tmp->sname[1] = (char *)malloc(sizeof(char)*strlen(pagina[j].valorCampo) * 2);
							//	printf("Aloquei 2\n");
							}
							//printf("Nome2: %s\n", st->filters[position].right);
							st->filters[position].right[strlen(st->filters[position].right)] = '\0';
							strcpy(tmp->sname[1], st->filters[position].right);
							//printf("Nome2: %s\n", tmp->sname[1]);
							tmp->typeValue = 'C';
						}

						else if(st->filters[position].typeAtt == 'D'){
							tmp->dvalue[1] = atof(st->filters[position].right);
							tmp->typeValue = 'D';
						}

						else if(st->filters[position].typeAtt == 'I'){
							tmp->ivalue[1] = atoi(st->filters[position].right);
							tmp->typeValue = 'I';
						}
					}
					else if(strcmp(pagina[j].nomeCampo, st->filters[position].right) == 0){
						*(poAtt+n) = j+objeto.qtdCampos;
						//poAtt[n] = j+objeto.qtdCampos; COMENTEI AQUI
						n++;
						full=2;
						flag=1;
						//Agora salvar na lista Value.
						if(pagina[j].tipoCampo == 'S' || pagina[j].tipoCampo == 'C'){
							if(valido == -10)
								tmp->sname[1] = (char *)malloc(sizeof(char)*esquema->tam + sizeof(char)*2);
							pagina[j].valorCampo[strlen(pagina[j].valorCampo)] = '\0';
							strcpy(tmp->sname[1], pagina[j].valorCampo);
							tmp->typeValue = 'C';
						}

						else if(pagina[j].tipoCampo == 'D'){
							auxd = (double *)&pagina[j].valorCampo[0];
							tmp->dvalue[1] = *auxd;
							tmp->typeValue = 'D';
						}
						else if(pagina[j].tipoCampo == 'I'){
							auxi = (int *)&pagina[j].valorCampo[0];
							tmp->ivalue[1] = *auxi;
							tmp->typeValue = 'I';
						}
				
					}
				}
				//Se verdadeiro. Preciso fazer tudo de novo hehe
				if(full > 0 && flag == 1){
					j=-1;
					//Quero que apenas quando full=2
					if(full == 2){//Quando full=2, é sinal que preciso criar novo value, e setar o próximo filtro. Pois tudo já foi preenchido
						tmp->typeLogic = st->filters[position].typeLogico;
						tmp->typeOp = st->filters[position].typeOp;
						full = 0;
						tmp = tmp->next;
						if(valido == -10)//Só vai acontecer no 1° registro, para alocar a quantia exata de values
							tmp->next =  (list_value *) malloc(sizeof(list_value)); //próxima lista de values
						position++; //Próximo filtro
					}
				}

			}//for
			//Acabei de ler o primeiro registro.. primeira tupla
			primeiraTupla=0;
			valido = selectWhere(value);//Já posso chamar o selectWhere.c
			count++;
			if(valido){
				nvalidas++;
				if(nvalidas == 1)
					printCabecalho(pagina, st->projection, st->nprojection);
				//atualiaPoProj(j, objeto.qtdCampo, st->nprojection, poProj, positions); 
				if(save){
					printSaveTupla(i, auxpag, st->projection, st->nprojection, registros);
					save = 0;				
				}
				printTupla(0, pagina, st->projection, st->nprojection, registros);
			}
		}//If primeira tupla
/*********************************
Até aqui fiz apenas pro primeiro registro. E claro, salvei a posição dos atributos para serem lidos no 'vetor' poAtt(tem que testar).
Agora eh só pegar o resto dos registros, acessá-los diretamente. Salvar no *value e jogar no select
*************************************/

		//Se não for primeira tupla vem aqui.		
		if(primeiraTupla == 0){
			tmp = value;//Preciso voltar a apontar pro inicio do da Lista Value
			tmp->next = value->next;
			position = 0;
			//for(position=0; *(poAtt+m) < registros; j++){//O valor no vetor deverá ser menor que a quantidade de registros
			while(*(poAtt+m) < registros){
				if(st->filters[position].typeAtt == 'C'){
					if(full == 0){
						if(*(poAtt+m) == -1) //Caso for -1, valor vem direto do operador ESQUERDO no where
							strcpy(tmp->sname[0], st->filters[position].left);
						else{//Se não, ele vem da página
							strcpy(tmp->sname[0], pagina[*(poAtt+m)].valorCampo);
							//printf("Valo2: %d\n",*(poAtt+m));					
							*(poAtt+m) += objeto.qtdCampos;	
						}
						full = 1;//Já achei o atributo da esquerda
					}
					if(*(poAtt+m+1) < registros){
						if(*(poAtt+m+1) == -2){//Caso seja -2, atributo vem direto do operador DIREITO no where
							strcpy(tmp->sname[1], st->filters[position].right);
							//printf("Nome12: %s\n", tmp->sname[1]);
						}
						else{//Se não, vem da página
							strcpy(tmp->sname[1], pagina[*(poAtt+m+1)].valorCampo);
							*(poAtt+m+1) += objeto.qtdCampos;
						}
						full=2;
					}
					else
						save=1;
				
				}

				else if(st->filters[position].typeAtt == 'D'){
					if(full == 0){
						if(*(poAtt+m) == -1)
							tmp->dvalue[0] = atof(st->filters[position].left);
						else{
							auxd = (double *)&pagina[*(poAtt+m)].valorCampo[0];
							tmp->dvalue[0] = *auxd;
							*(poAtt+m) += objeto.qtdCampos;
						}
						full = 1;
					}
					if(*(poAtt+m+1) < registros){
						if(*(poAtt+m+1) == -2)
							tmp->dvalue[1] = atof(st->filters[position].right);
						else{
							auxd = (double *)&pagina[*(poAtt+m+1)].valorCampo[0];
							tmp->dvalue[1] = *auxd;
							*(poAtt+m+1) += objeto.qtdCampos;
						}
						full=2;
					}
					else
						save=1;
				}

				else if(st->filters[position].typeAtt == 'I'){
					if(full == 0){
						if(*(poAtt+m) == -1)
							tmp->ivalue[0] = atoi(st->filters[position].left);
						else{
							auxi = (int *)&pagina[*(poAtt+m)].valorCampo[0]; //Aqui deu pau.. preciso ler outra página.
							tmp->ivalue[0] = *auxi;
							*(poAtt+m) += objeto.qtdCampos;
						}
						full = 1;
					}
					if(*(poAtt+m+1) < registros){
						if(*(poAtt+m+1) == -2){
							tmp->ivalue[1] = atoi(st->filters[position].right);
						}
						else{
							auxi = (int *)&pagina[*(poAtt+m+1)].valorCampo[0];
							tmp->ivalue[1] = *auxi;
							*(poAtt+m+1) += objeto.qtdCampos;	
						}
						full=2;
					}
					else
						save=1;
				}
				if(position == 0){
					if(*(poAtt+m) == -1)
						j = *(poAtt+m+1) - objeto.qtdCampos;
					j = *(poAtt+m) - objeto.qtdCampos;
				}

				if(full == 2){ //Encheu a lista, setar para a  nova
					tmp->typeLogic = st->filters[position].typeLogico;
					tmp->typeOp = st->filters[position].typeOp;
					tmp->typeValue = st->filters[position].typeAtt;
					position++;
					m += 2;
					full=0;
					
					
					tmp = tmp->next; //proxima lista
					if(m == n){//Se verdadeiro, terminou um registro
						valido = selectWhere(value);
						//printf("valido2: %d\n",valido);
						count++;
						if(valido){
							nvalidas++;
							if(nvalidas == 1)
								printCabecalho(pagina, st->projection, st->nprojection);
							printTupla(j, pagina, st->projection, st->nprojection, registros);
							//printf("save: %d\n", save);
						}
						m=0;
						tmp = value;
						tmp->next = value->next;
						position=0;		
					}
				}
			}//while(*(poAtt+m) < registros)
			primeiraTupla=1;
		}//IF primeira==0;
	n=0;
	x -= bufferpoll[p++].nrec;



	}//While(x)
	printf("(%d rows)\n\n",nvalidas);

}//Fim função.





























