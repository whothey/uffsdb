#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct list_value{
	char	typeValue;
	char	typeOp;
	char	typeLogic;
	int		ivalue[1];
	double	dvalue[1];
	char	*sname[2];
	struct list_value *next;
}list_value;


int main(){



	return 0;
}

//Não terminei de achar detalhe nessa coisa velho ehuahuehuae
//Foi tenso!

void doSelect(rc_select *st){

	int i,erro,x,p,ntuplas,position=0,flag=1,count=0,primeiraTupla=1,valido;
	int *poAtt, n=0;
	struct fs_objects objeto;
	list_value *value = (list_value *) malloc(sizeof(list_value);
	value->next = (list_value *) malloc(sizeof(list_value);

    if(!verificaNomeTabela(st->tables)){ //Verificação nome da Tabela
        printf("\nERROR: relation \"%s\" was not found.\n\n\n", nomeTabela);
        return;
    }

    objeto = leObjeto(st->tables); 

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
	poAtt = (int *) malloc(sizeof(int)*objeto.qtdCampos); //No máximo terá o tamanho igual a quantidade de campos.

	//Coloca todas as tuplas daquela tabela no buffer
	erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);

	list_value *tmp = value;
	tmp->next = value_next;
	ntuples = --x;
	p = 0;

	//Vou começar a ler e salvar na struct *value
	while(x){
		//Lê uma página p do buffer e salva na variavel *pagina
		column *pagina = getPage(bufferpoll, esquema, objeto, p);
	    if(pagina == ERRO_PARAMETRO){
            printf("ERROR: could not open the table.\n");
            free(bufferpoll);
            free(esquema);
            return;
	    }
		if(primeiraTupla){
		//Quando for a 1° página, vou varrer todo o objeto, encontrar as posições certinhas, salvar no *poAtt. E na próxima página acessar tudo tipo ninja
			for(j=0; j < objeto.qtdCampos; j++){
				//Verificar se já achou o atributo da esquerda, caso count>0 eh porque já achou.
				//Mais rápido perguntar se count == 0 do que perguntar a strcmp se pagi..... são iguais
				if(count == 0){
					//Se lado esquero não for um atributo, mas sim um valor
					if(caso_st->filters[position].left) == valor){
						*(poAtt+n) = -1; //Quando for -1 eh porque o valor vem direto do filters.left
						n++;
						count++;
						if(st->filters[position].type == 'C'){
							strcpy(tmp->sname[1], st->filters[position].left);
							tmp->typeValue = 'C';
						}

						else if(st->filters[position].type == 'D'){
							tmp->dvalue[0] = atof(st->filters[position].left);
							tmp->typeValue = 'D';
						}

						else if(st->filters[position].type == 'I'){
							tmp->ivalue[0] = atoi(st->filters[position].left);
							tmp->typeValue = 'I';
						}
					}

					//Se nomes são iguais volta 0
					if(strcmp(pagina[j]nomeCampo, st->filters[position].left) == 0){
						//N eh pra controlar o 'vetor' poAtt
						//Cara, essa linha tá sinistra, preciso te explicar falando. Espero que funcione heuha
						if(j == 0)
							*(poAtt+n) = objeto.qtdCampos+1;//Qualquer coisa preciso tirar esse +1
						else
							*(poAtt+n) = j*objeto.qtdCampos+1;
							
						n++;
						count++;
						//Agora salvar na lista Value.
						if(pagina[j].tipoCampo == 'S' || pagina[j].tipoCampo == 'C'){
							strcpy(tmp->sname[0], pagina[j].valorCampo);
							tmp->typeValue = 'C';
						}

						else if(pagina[j].tipoCampo == 'D'){
							tmp->dvalue[0] = pagina[j].ValorCampo;
							tmp->typeValue = 'D';
						}

						else if(pagina[j].tipoCampo == 'I'){
							tmp->ivalue[0] = pagina[j].ValorCampo;
							tmp->typeValue = 'I';
						}
					}
				}

				//Count>0 sinal que já encontrou o att da esquerda. Antes disso não pode achar o da direita
				else if(count > 0){
					flag=0;
					//Se o lado direito não for um atributo, mas sim um valor
					if(caso_st->filters[position].right) == valor){
						*(poAtt+n) = -2; //Quando for -1 eh porque o valor vem direto do filters.right
						n++;
						count++;
						flag=1;
						if(st->filters[position].type == 'C'){
							strcpy(tmp->sname[1], st->filters[position].right);
							tmp->typeValue = 'C';
						}

						else if(st->filters[position].type == 'D'){
							tmp->dvalue[1] = atof(st->filters[position].right);
							tmp->typeValue = 'D';
						}

						else if(st->filters[position].type == 'I'){
							tmp->ivalue[1] = atoi(st->filters[position].right);
							tmp->typeValue = 'I';
						}
					}
					else if(strcmp(pagina->nomeCampo, st->filters[position].right) == 0){
						//Cara, essa linha tá sinistra, preciso te explicar falando. Espero que funcione heuha
						if(j == 0)
							*(poAtt+n) = objeto.qtdCampos+1;//Qualquer coisa preciso tirar esse +1
						else
							*(poAtt+n) = j*objeto.qtdCampos+1;
						n++;
						count++;
						flag=1;
						//Agora salvar na lista Value.
						if(pagina[j].tipoCampo == 'S' || pagina[j].tipoCampo == 'C'){
							strcpy(tmp->sname[1], pagina[j].valorCampo);
							tmp->typeValue = 'C';
						}

						else if(pagina[j].tipoCampo == 'D'){
							tmp->dvalue[1] = pagina[j].ValorCampo;
							tmp->typeValue = 'D';
						}
						else if(pagina[j].tipoCampo == 'I'){
							tmp->ivalue[1] = pagina[j].ValorCampo;
							tmp->typeValue = 'I';
						}
				
					}
				}
				//Se verdadeiro. Preciso fazer tudo de novo hehe
				if(count >= 1 && flag == 1){
					j=-1;
					//Quero que apenas quando count=2
					if(count > 1)){//Quando count=2, é sinal que preciso criar novo value, e setar o próximo filtro. Pois tudo já foi preenchido
						tmp->typeLogic = st->filters[position].typeLogic;
						tmp->typeOp = st->filters[position].typeOp;
						j=-1;
						count=0;//Quero voltar a preencher o filtro.left
						tmp = tmp->next;
						tmp->next =  (list_value *) malloc(sizeof(list_value)); //próxima lista de values
						position++; //Próximo filtro
					}
				}

			}
			//Acabei de ler o primeiro registro.. primeira tupla
			primeiraTupla=0;
			//Já posso chamar o selectWhere.c
			valido = selectWhere(value);			
			if(valido) //Se valido=1 posso imprimir. Caso valido=0 não posso.
				//IMPRIMIIIRR ESSA TUUPLAAA HEUHA
		}
/*********************************
Até aqui fiz apenas pro primeiro registro. E claro, salvei a posição dos atributos para serem lidos no 'vetor' poAtt(tem que testar).
Agora eh só pegar o resto dos registros, acessá-los diretamente. Salvar no *value e jogar no select
*************************************/
		
		for(j=0; j < objeto.qtdCampos*bufferpoll[p].nrec; j++){
			//Caso eh a primeira página, vou acessar com força bruta e salvar no poAtt.




		}

	}

}





























