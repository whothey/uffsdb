#include "buffend.h"

/* Funções auxiliares do insert. */

// Verifica se dois tipos de dados são compatíveis, ou seja, se table_type pode receber
// insert_type, mesmo não sendo exatamente o tipo especificado
int typesCompatible(char table_type, char insert_type);

// Busca o valor na inserção *s_insert designado à *columnName.
// Se não existe, retorna 0, 0.0 ou \0
char *getInsertedValue(rc_insert *s_insert, char *columnName, table *tabela);

// Busca o tipo do valor na inserção *s_insert do valor que irá para *columnName
// Se não existe em *s_insert, assume o tipo do esquema já que não receberá nada.
char getInsertedType(rc_insert *s_insert, char *columnName, table *tabela);

// Se foram especificadas colunas no *s_insert, verifica se elas existem no esquema.
int allColumnsExists(rc_insert *s_insert, table *tabela);

void insert(rc_insert *s_insert) {
	int i;
	table *tabela = (table *)malloc(sizeof(table));
	tabela->esquema = NULL;
	memset(tabela, 0, sizeof(table));
	column *colunas = NULL;
	tp_table *esquema = NULL;
	struct fs_objects objeto;
	memset(&objeto, 0, sizeof(struct fs_objects));
	char  flag=0;

	abreTabela(s_insert->objName, &objeto, &tabela->esquema); //retorna o esquema para a insere valor
	strcpylower(tabela->nome, s_insert->objName);

	if(s_insert->columnName != NULL) {
		if (allColumnsExists(s_insert, tabela)) {
			for (esquema = tabela->esquema; esquema != NULL; esquema = esquema->next) {
				if(typesCompatible(esquema->tipo,getInsertedType(s_insert, esquema->nome, tabela))) {
					colunas = insereValor(tabela, colunas, esquema->nome, getInsertedValue(s_insert, esquema->nome, tabela));
					
					if (!colunas) { //Caso retorne null no insereValor
							
							flag = 1;
					
					}
					
				} else {
					printf("ERROR: data type invalid to column '%s' of relation '%s' (expected: %c, received: %c).\n", esquema->nome, 
							tabela->nome, esquema->tipo, getInsertedType(s_insert, esquema->nome, tabela));
					flag=1;
				}
			}
		} else {
			flag = 1;
		}
	} else {
		if (s_insert->N == objeto.qtdCampos) {
			for(i=0; i < objeto.qtdCampos; i++) {

				if(s_insert->type[i] == 'S' && tabela->esquema[i].tipo == 'C') {
					s_insert->values[i][1] = '\0';
					s_insert->type[i] = 'C';
				}

				if(s_insert->type[i] == 'I' && tabela->esquema[i].tipo == 'D') {

					s_insert->type[i] = 'D';
				}

				if(s_insert->type[i] == tabela->esquema[i].tipo){
					colunas = insereValor(tabela, colunas, tabela->esquema[i].nome, s_insert->values[i]); 
					if(colunas == NULL){
						flag = 1;
						printf("ERROR: Oversized of column '%s' of relation '%s'\n", tabela->esquema[i].nome, tabela->nome);
						return;	
					}
				}else {
					printf("ERROR: data type invalid to column '%s' of relation '%s' (expected: %c, received: %c).\n", 
						tabela->esquema[i].nome, tabela->nome, tabela->esquema[i].tipo, s_insert->type[i]);
					flag = 1;
				}
			}
		} else {
			printf("ERROR: INSERT has more expressions than target columns.\n");
			flag = 1;
		}
	}

	if (!flag)
		if (finalizaInsert(s_insert->objName, colunas) == SUCCESS)
			printf("INSERT 0 1\n");

	//freeTp_table(&esquema, objeto.qtdCampos);
	free(esquema);
	freeColumn(colunas);
	freeTable(tabela);
}

int typesCompatible(char table_type, char insert_type) {
	return (table_type == 'D' && insert_type == 'I')
		|| (table_type == 'D' && insert_type == 'D')
		|| (table_type == 'I' && insert_type == 'I')
		|| (table_type == 'S' && insert_type == 'S')
		|| (table_type == 'S' && insert_type == 'C')
		|| (table_type == 'C' && insert_type == 'C')
		|| (table_type == 'C' && insert_type == 'S');
}

// Busca o valor na inserção *s_insert designado à *columnName.
// Se não existe, retorna \0
char *getInsertedValue(rc_insert *s_insert, char *columnName, table *tabela) {
	int i;
	//char tipo, *noValue;

	for (i = 0; i < s_insert->N; i++) {
		if (objcmp(s_insert->columnName[i], columnName) == 0) {
			return s_insert->values[i];
		}
	}

	return '\0';
}

// Busca o tipo do valor na inserção *s_insert do valor que irá para *columnName
// Se não existe em *s_insert, assume o tipo do esquema já que não receberá nada.
char getInsertedType(rc_insert *s_insert, char *columnName, table *tabela) {
	int i;
	char noValue;
	for (i = 0; i < s_insert->N; i++) {
		if (objcmp(s_insert->columnName[i], columnName) == 0) {
			return s_insert->type[i];
		}
	}

	noValue = retornaTamanhoTipoDoCampo(columnName, tabela);

	return noValue;
}

// Se foram especificadas colunas no *s_insert, verifica se elas existem no esquema.
int allColumnsExists(rc_insert *s_insert, table *tabela) {
	int i;
	if (!s_insert->columnName) return 0;

	for (i = 0; i < s_insert->N; i++)
		if (retornaTamanhoTipoDoCampo(s_insert->columnName[i], tabela) == 0) {
			printf("ERROR: column \"%s\" of relation \"%s\" does not exist.\n", s_insert->columnName[i], tabela->nome);
			return 0;
		}

	return 1;
}
