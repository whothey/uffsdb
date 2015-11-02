//cria tabela
#include "buffend.h"


int verifyFK(char *tableName, char *column){
    if(verificaNomeTabela(tableName) == 1){
        struct fs_objects objeto = leObjeto(tableName);
        tp_table *esquema = leSchema(objeto);
		if(esquema == ERRO_ABRIR_ESQUEMA){
            printf("ERROR: cannot create schema.\n");
            return 0;
        }

        for(; esquema != NULL; esquema = esquema->next){
            if(objcmp(esquema->nome, column) == 0){
                if(esquema->chave == PK){
                    return 1;
                }
            }
        }
    }
	return 0;
}

int verifyFieldName(char **fieldName, int N){
    int i, j;
    if(N<=1) return 1;
    for(i=0; i < N; i++){
        for(j=i+1; j < N; j++){
            if(objcmp(fieldName[i], fieldName[j]) == 0){
                printf("ERROR: column \"%s\" specified more than once\n", fieldName[i]);
                return 0;
            }
        }
    }
    return 1;
}

void createTable(rc_insert *t) {
	int size;
    strcpylower(t->objName, t->objName);        //muda pra minúsculo
    char *tableName = (char*) malloc (sizeof(char)*TAMANHO_NOME_TABELA),
        fkTable[TAMANHO_NOME_TABELA], fkColumn[TAMANHO_NOME_CAMPO];

    strncpylower(tableName, t->objName, TAMANHO_NOME_TABELA);

    strcat(tableName, ".dat\0");                  //tableName.dat

    if(existeArquivo(tableName)){
        printf("ERROR: table already exist\n");
        free(tableName);
        return;
    }

    table *tab = NULL;
    tab = iniciaTabela(t->objName);    //cria a tabela

    if(0 == verifyFieldName(t->columnName, t->N)){
        free(tableName);
        freeTable(tab);
        return;
    }
    int i;
    for(i=0; i < t->N; i++){
    	if (t->type[i] == 'S'){
    		size = atoi(t->values[i]);
    		if(size<1){
			printf("Error: The size of varchar has to be %s. ", (size==0? "greater than zero" : (t->values[i][0]=='-'? "positive": "smaller")));  
			if(size<0&&t->values[i][0]!='-')
				printf("Varchar(%s) is not allowed.\n", t->values[i]);
			else
				printf("Varchar(%d) is not allowed.\n", size); 
			free(tableName);
               		freeTable(tab);
			return;		
		}
    	}
    	else if (t->type[i] == 'I')
    		size = sizeof(int);
    	else if (t->type[i] == 'D')
    		size = sizeof(double);
        else if (t->type[i] == 'C')
    		size = sizeof(char);

    	if (t->attribute[i] == FK) {
    		strncpylower(fkTable, t->fkTable[i], TAMANHO_NOME_TABELA);
    		strncpylower(fkColumn,t->fkColumn[i], TAMANHO_NOME_CAMPO);
    	} else {
    		strcpy(fkTable, "");
    		strcpy(fkColumn, "");
    	}

        tab = adicionaCampo(tab, t->columnName[i], t->type[i], size, t->attribute[i], fkTable, fkColumn);
        if((objcmp(fkTable, "") != 0) || (objcmp(fkColumn, "") != 0)){
            if(verifyFK(fkTable, fkColumn) == 0){
    			printf("ERROR: attribute FK cannot be referenced\n");
                free(tableName);
                freeTable(tab);
                return;
    		}
        }
    }

    printf("%s\n",(finalizaTabela(tab) == SUCCESS)? "CREATE TABLE" : "ERROR: table already exist\n");
    free(tableName);
    if (tab != NULL) freeTable(tab);
}
