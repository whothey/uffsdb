#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int verificaNomeAtributo(char * nTabela, char *nAtributo){
	FILE *dicionario;
	char *att = (char *)malloc(sizeof(char)*40); //TAMANHO_NOME_CAMPO=40
	
	char directory[LEN_DB_NAME*2];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_schema.dat");

	if((dicionario = fopen(directory,"a+b")) == NULL){
        free(att);
        return ERRO_ABRIR_ARQUIVO;
    }
	int cod = cod_tabela(nTabela);
	int seekCount=0, newCod;
	
	while(fgetc(dicionario) != EOF){
		fseek(dicionario, seekCount, SEEK_SET);	
		
		fread(&newCod, sizeof(int), 1, dicionario);
		
			if(newCod == cod){
				seekCount += 4;
				fseek(dicionario, seekCount, SEEK_SET);
				
				fread(att, sizeof(char), 40, dicionario);
				
				if(strcmp(nAtributo,att) == 0){
					free(att);
					fclose(dicionario);
					return 1;
				}
			}
		seekCount += 45;
		//Baseado no dicionario do fs_schema.dat
	}
	fclose(dicionario);
	free(att);
	return 0;
}


int cod_tabela(char *nTabela){
	
	FILE *dicionario;
    char *tupla = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);
	int cod;

    char directory[LEN_DB_NAME*2];
    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_object.dat");


    if((dicionario = fopen(directory,"a+b")) == NULL){
        free(tupla);
        return ERRO_ABRIR_ARQUIVO;
    }

    while(fgetc (dicionario) != EOF){
        fseek(dicionario, -1, 1);

        fread(tupla, sizeof(char), TAMANHO_NOME_TABELA, dicionario); //Lê somente o nome da tabela

        if(objcmp(tupla, nomeTabela) == 0){ // Verifica se o nome dado pelo usuario existe no dicionario de dados.
			fread(&cod, sizeof(int), 1, dicionario);
            free(tupla);
            fclose(dicionario);
            return cod;
        }

        fseek(dicionario, 28, 1);
    }

    fclose(dicionario);
    free(tupla);

    return -1;
}
