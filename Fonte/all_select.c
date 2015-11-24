#include "buffend.h"

void doSelect(qr_select *st){
	printf("ola");
	int j=0,erro,x,p,position=0,flag=1,primeiraTupla=1,valido=-10,m,aux=j;
	//ntuplas está ali pra saber quantas tuplas foram impressas.
	int *poAtt,n; //,*auxPoAtt;
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
	poAtt = (int *)malloc(sizeof(int)*objeto.qtdCampos); //No máximo terá o tamanho igual a quantidade de campos.
	//auxPoAtt = (int *)malloc(sizeof(int)*objeto.qtdCampos);

	//Coloca todas as tuplas daquela tabela no buffer
	erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);
	list_value *tmp = value;
	tmp->next = value->next;
	int ntuplas = --x,full=0;
	p = 0;
	
	//Vou começar a ler e salvar na struct *value
	while(x){
		//Lê uma página p do buffer e salva na variavel *pagina
		//j=0;
		printf("ola!");
		column *pagina = getPage(bufferpoll, esquema, objeto, p);
	    if(pagina == ERRO_PARAMETRO){
            printf("ERROR: could not open the table.\n");
            free(bufferpoll);
            free(esquema);
            return;
	    }
		printf("ola!2");
		if(primeiraTupla){

		//Quando for a 1° tupla, vou varrer todo o objeto, encontrar as posições certinhas, salvar no *poAtt. E na próxima página acessar tudo tipo ninja
			for(j=0; st->nfilters > position; j++){
				//Verificar se já achou o atributo da esquerda, caso full=1 eh porque já achou.
				//Mais rápido perguntar se count == 0 do que perguntar a strcmp se pagi..... são iguais
				//printf("OP: %c\nPosition: %d\n", st->filters[position].typeOp, position);
				if(full == 0){
					//Se lado esquero não for um atributo, mas sim um valor
					if(st->filters[position].left_type == 'V'){
						*(poAtt+n) = -1; //Quando for -1 eh porque o valor vem direto do filters.left
						//*(auxPoAtt+n) = -1;
						n++;
						//count++;
						full=1;
						if(st->filters[position].typeAtt == 'C'){
							strcpy(tmp->sname[0], st->filters[position].left);
							tmp->typeValue = 'C';
						}

						else if(st->filters[position].typeAtt == 'D'){
							tmp->dvalue[0] = atof(st->filters[position].left);
							tmp->typeValue = 'D';
						}

						else if(st->filters[position].typeAtt == 'I'){
							tmp->ivalue[0] = atoi(st->filters[position].left);
							tmp->typeValue = 'I';
						}
					}

					//Se nomes são iguais volta 0
					else if(strcmp(pagina[j].nomeCampo, st->filters[position].left) == 0){
						//N eh pra controlar o 'vetor' poAtt
						//Cara, essa linha tá sinistra, preciso te explicar falando. Espero que funcione heuha
					//	*(poAtt+n) = j+objeto.qtdCampos;
						//*(auxPoAtt+n) = j+objeto.qtdCampos;							
						n++;
						//count++;
						full=1;
						//Agora salvar na lista Value.
						if(pagina[j].tipoCampo == 'S' || pagina[j].tipoCampo == 'C'){
							strcpy(tmp->sname[0], pagina[j].valorCampo);
							printf("Value1: %s\n",tmp->sname[0]);
							tmp->typeValue = 'C';
						}

						else if(pagina[j].tipoCampo == 'D'){
							auxd = (double *)&pagina[j].valorCampo[0];
							tmp->dvalue[0] = *auxd;
							tmp->typeValue = 'D';
						}

						else if(pagina[j].tipoCampo == 'I'){
							auxi = (int *)&pagina[j].valorCampo[0];
							//printf("\nValor1 Int: %d\n",*auxi);
							tmp->ivalue[0] = *auxi;
							tmp->typeValue = 'I';
						}
					}
				}

				//full = 1 sinal que já encontrou o att da esquerda. Antes disso não pode achar o da direita
				else if(full == 1){
					flag=0;
					//Se o lado direito não for um atributo, mas sim um valor
					if(st->filters[position].right_type == 'V'){
						*(poAtt+n) = -2; //Quando for -2 eh porque o valor vem direto do filters.right
						//*(auxPoAtt+n) = -2;
						n++;
						//count++;
						full = 2;
						flag=1;
						if(st->filters[position].typeAtt == 'C'){
							strcpy(tmp->sname[1], st->filters[position].right);
							tmp->typeValue = 'C';
						}

						else if(st->filters[position].typeAtt == 'D'){
							tmp->dvalue[1] = atof(st->filters[position].right);
							tmp->typeValue = 'D';
						}

						else if(st->filters[position].typeAtt == 'I'){
							tmp->ivalue[1] = atoi(st->filters[position].right);
							printf("Valor2 Int: %d\n",tmp->ivalue[1]);
							tmp->typeValue = 'I';
						}
					}
					else if(strcmp(pagina[j].nomeCampo, st->filters[position].right) == 0){
						//Cara, essa linha tá sinistra, preciso te explicar falando. Espero que funcione heuha
//						printf("\nAQUI\n");
						*(poAtt+n) = j+objeto.qtdCampos;
						//*(auxPoAtt+n) = j+objeto.qtdCampos;
						n++;
						//count++;
						full=2;
						flag=1;
						//Agora salvar na lista Value.
						if(pagina[j].tipoCampo == 'S' || pagina[j].tipoCampo == 'C'){
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
					aux=j;
					j=-1;
					//Quero que apenas quando count=2
					if(full == 2){//Quando full=2, é sinal que preciso criar novo value, e setar o próximo filtro. Pois tudo já foi preenchido
						tmp->typeLogic = st->filters[position].typeLogico;
						tmp->typeOp = st->filters[position].typeOp;
						//count=0;//Quero voltar a preencher o filtro.left
						full = 0;
						tmp = tmp->next;
						if(valido == -10)
							tmp->next =  (list_value *) malloc(sizeof(list_value)); //próxima lista de values
						position++; //Próximo filtro
						if(position < st->nfilters)
							j=-1;
						else
							j=aux;
					}
				}

			}
			//Acabei de ler o primeiro registro.. primeira tupla
			primeiraTupla=0;
			//Já posso chamar o selectWhere.c
			printf("\nlogic: %c\nivalue1: %d\nivalue2: %d\nOp: %c\n", value->typeLogic, value->ivalue[0],value->ivalue[1], value->typeOp);
			valido = selectWhere(value);
			printf("valido1: %d\n",valido);
//			if(valido) //Se valido=1 posso imprimir. Caso valido=0 não posso.
	//			printf("Valido tupla n: %d",j);
				//IMPRIMIIIRR ESSA TUUPLAAA HEUHA
		}//If primeira tupla
/*********************************
Até aqui fiz apenas pro primeiro registro. E claro, salvei a posição dos atributos para serem lidos no 'vetor' poAtt(tem que testar).
Agora eh só pegar o resto dos registros, acessá-los diretamente. Salvar no *value e jogar no select
*************************************/
		//Se não for primeira tupla vem aqui.
		
		if(primeiraTupla == 0){
//			printf("\naqui fora"); m=0;
			tmp = value;
			tmp->next = value->next;
			for(position=0; *(poAtt+m) < objeto.qtdCampos * bufferpoll[p].nrec; j++){
				if(st->filters[position].typeAtt == 'C'){
					if(*(poAtt+m) == -1)
						strcpy(tmp->sname[0], st->filters[position].left);
					else{
						strcpy(tmp->sname[0], pagina[*(poAtt+m)].valorCampo);
						*(poAtt+m) += objeto.qtdCampos;
					}
					full = 1;
					if(*(poAtt+m+1) < objeto.qtdCampos * bufferpoll[p].nrec){
						if(*(poAtt+m+1) == -2)
							strcpy(tmp->sname[1], st->filters[position].right);
						else{
							strcpy(tmp->sname[1], pagina[*(poAtt+m+1)].valorCampo);
							*(poAtt+m+1) += objeto.qtdCampos;
						}
						full=2;
					}
				}

				else if(st->filters[position].typeAtt == 'D'){
					if(*(poAtt+m) == -1)
						tmp->dvalue[0] = atof(st->filters[position].left);
					else{
						auxd = (double *)&pagina[*(poAtt+m)].valorCampo[0];
						tmp->dvalue[0] = *auxd;
						*(poAtt+m) += objeto.qtdCampos;
					}
					full = 1;
					if(*(poAtt+m+1) < objeto.qtdCampos * bufferpoll[p].nrec){
						if(*(poAtt+m+1) == -2)
							tmp->dvalue[1] = atof(st->filters[position].right);
						else{
							auxd = (double *)&pagina[*(poAtt+m+1)].valorCampo[0];
							tmp->dvalue[1] = *auxd;
							*(poAtt+m+1) += objeto.qtdCampos;
						}
						full=2;
					}
				}

				else if(st->filters[position].typeAtt == 'I'){
					if(*(poAtt+m) == -1)
						tmp->ivalue[0] = atoi(st->filters[position].left);
					else{
						auxi = (int *)&pagina[*(poAtt+m)].valorCampo[0]; //Aqui deu pau.. preciso ler outra página.
						tmp->ivalue[0] = *auxi;
						*(poAtt+m) += objeto.qtdCampos;
					}
					full = 1;
					if(*(poAtt+m+1) < objeto.qtdCampos * bufferpoll[p].nrec){
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
						printf("\n2\nlogic: %c\nivalue1: %d\nivalue2: %d\nOp: %c\n", value->typeLogic, value->ivalue[0],value->ivalue[1], value->typeOp);
						m=0;
						valido = selectWhere(value);
						printf("valido: %d\n",valido);
						if(valido)
							printf("Valido tupla n: %d",j);
							//Imprimiiiirrr
						tmp = value;
						tmp->next = value->next;
						position=0;		
					}
				}
				
				//printf("\n2\nPosition: %d\nm= %d\nn= %d\nLogico: %c\nOp: %c\nTypeValue: %c\nValue1: %d\nValue2: %d\n",position,m,n,value->typeLogic,value->typeOp,value->typeValue, value->ivalue[0],value->ivalue[1]);

			}//FOR
			primeiraTupla=1;
		}//Else
	printf("aqui12");
	x -= bufferpoll[p++].nrec;



	}//While(x)
	printf("Terminou kk\nCom tantas linhas %d\n",ntuplas);

}//Fim função.





























