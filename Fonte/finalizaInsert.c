//BufferPool
#include "buffend.h"

int finalizaInsert(char *nome, column *c){
    column *auxC, *temp;
    int i = 0, x = 0, t, erro, j = 0 , flag=0;
    FILE *dados;

    struct fs_objects objeto,dicio; // Le dicionario
    tp_table *auxT ; // Le esquema
    auxT = abreTabela(nome, &dicio, &auxT);

    table *tab     = (table *)malloc(sizeof(table));
    tp_table *tab2 = (tp_table *)malloc(sizeof(struct tp_table));
    memset(tab2, 0, sizeof(tp_table));

    tab->esquema = abreTabela(nome, &objeto, &tab->esquema);
    tab2 = procuraAtributoFK(objeto);

    for(j = 0, temp = c; j < objeto.qtdCampos && temp != NULL; j++, temp = temp->next){
        switch(tab2[j].chave){
            case NPK:
                erro = SUCCESS;
                break;

            case PK:
                erro = verificaChavePK(nome, temp , temp->nomeCampo, temp->valorCampo);
                if (erro == ERRO_CHAVE_PRIMARIA){
                    printf("ERROR: duplicate key value violates unique constraint \"%s_pkey\"\nDETAIL:  Key (%s)=(%s) already exists.\n", nome, temp->nomeCampo, temp->valorCampo);

					free(auxT); // Libera a memoria da estrutura.
					//free(temp); // Libera a memoria da estrutura.
					free(tab); // Libera a memoria da estrutura.
					free(tab2); // Libera a memoria da estrutura.
                    return ERRO_CHAVE_PRIMARIA;
                }                
                if (erro == ERRO_CHAVE_PRIMARIA_NULA){
                    printf("ERROR:  null value in column '%s' violates not-null constraint \"%s_pkey\"\nDETAIL: Primary Key (%s) is null.\n",temp->nomeCampo,nome,temp->nomeCampo);

                    free(auxT); // Libera a memoria da estrutura.
                    //free(temp); // Libera a memoria da estrutura.
                    free(tab); // Libera a memoria da estrutura.
                    free(tab2); // Libera a memoria da estrutura.
                    return ERRO_CHAVE_PRIMARIA_NULA;
                }

                break;

            case FK:
                if (tab2[j].chave == 2 && strlen(tab2[j].attApt) != 0 && strlen(tab2[j].tabelaApt) != 0){

                    erro = verificaChaveFK(nome, temp, tab2[j].nome, temp->valorCampo, tab2[j].tabelaApt, tab2[j].attApt);

                    if (erro != SUCCESS){
                        printf("ERROR: invalid reference to \"%s.%s\". The value \"%s\" does not exist.\n", tab2[j].tabelaApt,tab2[j].attApt,temp->valorCampo);

						free(auxT); // Libera a memoria da estrutura.
						free(temp); // Libera a memoria da estrutura.
                        free(tab); // Libera a memoria da estrutura.
						free(tab2); // Libera a memoria da estrutura.
                        return ERRO_CHAVE_ESTRANGEIRA;
                    }
                }

                break;
        }
    }


    if (erro == ERRO_CHAVE_ESTRANGEIRA){
        printf("ERROR: unknown foreign key error.\n");

		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_CHAVE_ESTRANGEIRA;
    }

    if (erro == ERRO_CHAVE_PRIMARIA){
        printf("ERROR: unknown primary key error.\n");

		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_CHAVE_PRIMARIA;
    }
    if (erro == ERRO_DE_PARAMETRO) {
        printf("ERROR: invalid parameter.\n");
		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_DE_PARAMETRO;
    }

    char directory[LEN_DB_NAME*2];
    strcpy(directory, connected.db_directory);
    strcat(directory, dicio.nArquivo);

    if((dados = fopen(directory,"a+b")) == NULL){
        printf("ERROR: cannot open file.\n");
		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_ABRIR_ARQUIVO;

	}

    for(auxC = c, t = 0; auxC != NULL; auxC = auxC->next, t++){
        if (t >= dicio.qtdCampos)
            t = 0;

        if (auxT[t].tipo == 'S'){ // Grava um dado do tipo string.

            if (sizeof(auxC->valorCampo) > auxT[t].tam && sizeof(auxC->valorCampo) != 8){
                printf("ERROR: invalid string length.\n");
				free(tab); // Libera a memoria da estrutura.
				free(tab2); // Libera a memoria da estrutura.
				free(auxT); // Libera a memoria da estrutura.
				free(temp); // Libera a memoria da estrutura.
				fclose(dados);
                return ERRO_NO_TAMANHO_STRING;
            }

            if (objcmp(auxC->nomeCampo, auxT[t].nome) != 0){
                printf("ERROR: column name \"%s\" is not valid.\n", auxC->nomeCampo);
				free(tab); // Libera a memoria da estrutura.
				free(tab2); // Libera a memoria da estrutura.
				free(auxT); // Libera a memoria da estrutura.
				free(temp); // Libera a memoria da estrutura.
				fclose(dados);
                return ERRO_NOME_CAMPO;
            }

            char valorCampo[auxT[t].tam];
            strncpy(valorCampo, auxC->valorCampo, auxT[t].tam);
            strcat(valorCampo, "\0");
            fwrite(&valorCampo,sizeof(valorCampo),1,dados);

        } else if (auxT[t].tipo == 'I'){ // Grava um dado do tipo inteiro.
            i = 0;
            while (i < strlen(auxC->valorCampo)){
				if(auxC->valorCampo[i] == '-')
					flag=1;
               else if (auxC->valorCampo[i] < 48 || auxC->valorCampo[i] > 57){
                    printf("ERROR: column \"%s\" expectet integer.\n", auxC->nomeCampo);
					free(tab); // Libera a memoria da estrutura.
					free(tab2); // Libera a memoria da estrutura.
					free(auxT); // Libera a memoria da estrutura.
					//free(temp); // Libera a memoria da estrutura.
					fclose(dados);
                    return ERRO_NO_TIPO_INTEIRO;
                }
                i++;
            }

            int valorInteiro = 0;

            sscanf(auxC->valorCampo,"%d",&valorInteiro);
            fwrite(&valorInteiro,sizeof(valorInteiro),1,dados);

        } else if (auxT[t].tipo == 'D'){ // Grava um dado do tipo double.
            x = 0;
			flag=0;
            while (x < strlen(auxC->valorCampo)){
                if((auxC->valorCampo[x] < 48 || auxC->valorCampo[x] > 57) && (auxC->valorCampo[x] != 46)){
                    printf("ERROR: column \"%s\" expect double.\n", auxC->nomeCampo);
					free(tab); // Libera a memoria da estrutura.
					free(tab2); // Libera a memoria da estrutura.
					free(auxT); // Libera a memoria da estrutura.
					free(temp); // Libera a memoria da estrutura.
					fclose(dados);
                    return ERRO_NO_TIPO_DOUBLE;
                }
                x++;
            }
			double valorDouble;
			if(flag)
				valorDouble = atof(auxC->valorCampo);
			else
	            valorDouble = convertD(auxC->valorCampo);


            fwrite(&valorDouble,sizeof(valorDouble),1,dados);
        }
        else if (auxT[t].tipo == 'C'){ // Grava um dado do tipo char.

            if (strlen(auxC->valorCampo) > (sizeof(char))) {
                printf("ERROR: column \"%s\" expect char.\n", auxC->nomeCampo);
				free(tab); // Libera a memoria da estrutura.
				free(tab2); // Libera a memoria da estrutura.
				free(auxT); // Libera a memoria da estrutura.
				free(temp); // Libera a memoria da estrutura.
				fclose(dados);
                return ERRO_NO_TIPO_CHAR;
            }
            char valorChar = auxC->valorCampo[0];
            fwrite(&valorChar,sizeof(valorChar),1,dados);
        }

    }

    fclose(dados);
    free(tab); // Libera a memoria da estrutura.
	free(tab2); // Libera a memoria da estrutura.
    free(auxT); // Libera a memoria da estrutura.
    free(temp); // Libera a memoria da estrutura.
    return SUCCESS;
}
