//BufferPool
#include "buffend.h"

/*------------------------------------------------------------------------------------------
Objetivo: Mostrar as tabelas do banco de dados ou, em específico, os atributos de uma tabela
------------------------------------------------------------------------------------------*/

void printTable(char *tbl){
	if(tbl == NULL){     //mostra todas as tabelas do banco
		FILE *dicionario;
		printf("		List of Relations\n");
		char *tupla = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);

		char directory[LEN_DB_NAME*2];
    	strcpy(directory, connected.db_directory);
    	strcat(directory, "fs_object.dat");

		if((dicionario = fopen(directory,"a+b")) == NULL){
			free(tupla);
			printf("ERROR: cannot open file\n");
			return;
		}

		printf(" %-10s | %-15s | %-10s | %-10s\n", "Schema", "Name", "Type", "Owner");
		printf("------------+-----------------+------------+-------\n");
		int i=0;
		while(fgetc (dicionario) != EOF){
			fseek(dicionario, -1, 1);
			fread(tupla, sizeof(char), TAMANHO_NOME_TABELA, dicionario);
			printf(" %-10s | %-15s | %-10s | %-10s\n", "public", tupla, "tuple", "ibetres");
			fseek(dicionario, 28, 1);
			i++;
		}
		fclose(dicionario);
		free(tupla);
		printf("(%d %s)\n\n", i, (i<=1)? "row": "rows");
	} else{               //mostra todos atributos da tabela *tbl

		if(!verificaNomeTabela(tbl)) {
			printf("Did not find any relation named \"%s\".\n", tbl);
			return;
		}

		printf("	  Table \"public.%s\"\n", tbl);
		printf(" %-18s | %-12s | %-10s\n", "Column", "Type", "Modifiers");
		printf("--------------------+--------------+-----------\n");

		struct fs_objects objeto1;
		tp_table *esquema1;

		abreTabela(tbl, &objeto1, &esquema1);

		tp_table *tab3 = (tp_table *)malloc(sizeof(struct tp_table));
		tab3 = procuraAtributoFK(objeto1); //retorna tp_table
		int l, ipk=0, ifk=0;

		char **pk 			= (char**)malloc(objeto1.qtdCampos*sizeof(char**));
		char **fkTable		= (char**)malloc(objeto1.qtdCampos*sizeof(char**));
		char **fkColumn 	= (char**)malloc(objeto1.qtdCampos*sizeof(char**));
		char **refColumn 	= (char**)malloc(objeto1.qtdCampos*sizeof(char**));

		memset(pk 		, 0, objeto1.qtdCampos);
		memset(fkTable 	, 0, objeto1.qtdCampos);
		memset(fkColumn , 0, objeto1.qtdCampos);
		memset(refColumn, 0, objeto1.qtdCampos);

		int i;
		for(i=0; i<objeto1.qtdCampos; i++) {
			pk[i] 			= (char*)malloc(TAMANHO_NOME_CAMPO*sizeof(char));
			fkTable[i] 		= (char*)malloc(TAMANHO_NOME_CAMPO*sizeof(char));
			fkColumn[i] 	= (char*)malloc(TAMANHO_NOME_CAMPO*sizeof(char));
			refColumn[i] 	= (char*)malloc(TAMANHO_NOME_CAMPO*sizeof(char));

			memset(pk[i] 		, '\0', TAMANHO_NOME_CAMPO);
			memset(fkTable[i] 	, '\0', TAMANHO_NOME_CAMPO);
			memset(fkColumn[i]  , '\0', TAMANHO_NOME_CAMPO);
			memset(refColumn[i] , '\0', TAMANHO_NOME_CAMPO);

		}

		for(l=0; l<objeto1.qtdCampos; l++) {

			if(tab3[l].chave == PK){
				strcpylower(pk[ipk++], tab3[l].nome);
			}
			else if(tab3[l].chave == FK){
				strcpylower(fkTable[ifk]	, tab3[l].tabelaApt);
				strcpylower(fkColumn[ifk]	, tab3[l].attApt);
				strcpylower(refColumn[ifk++], tab3[l].nome);
			}

			printf("  %-17s |", tab3[l].nome);

			if(tab3[l].tipo == 'S')
				printf(" %-8s(%d) |", " varchar", tab3[l].tam);
			else if(tab3[l].tipo == 'I')
				printf(" %-10s   |", " integer");
			else if(tab3[l].tipo == 'C')
				printf(" %-10s   |", " char");
			else if(tab3[l].tipo == 'D')
				printf(" %-10s   |", " double");

			printf(" %-10s ", (tab3[l].chave == PK || tab3[l].chave == FK)? "not null": "null");

			printf("\n");
		}
		if(ipk){	//printf PK's
			printf("Indexes:\n");
			if(ipk > 1){
				printf("\t\"%s_pkey\" PRIMARY KEY (",tbl);
				for(l = 0; l < ipk-1; l++)
					printf("%s,",pk[l]);
				printf("%s)\n",pk[l] );

			
			}else
				for(l = 0; l < ipk; l++){
					printf("\t\"%s_pkey\" PRIMARY KEY (%s)\n", tbl, pk[l]);
			}
		}
		if(ifk){	//printf FK's
			printf("Foreign-key constrains:\n");
			for(l = 0; l < ifk; l++){
				printf("\t\"%s_%s_fkey\" FOREIGN KEY (%s) REFERENCES %s(%s)\n",tbl, refColumn[l], refColumn[l], fkTable[l], fkColumn[l]);
			}
		}

		free(pk);
		free(fkTable);
		free(fkColumn);
		free(refColumn);
		free(tab3);
		printf("\n");
	}
}
