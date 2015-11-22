#include <stdlib.h>
#include <pthread.h>
#include "../buffend.h"
#include "parser.h"
#include <string.h>


/* Estrutura global que guarda as informações obtidas pelo yacc
 * na identificação dos tokens
 */
rc_insert GLOBAL_DATA;

/* Estrutura auxiliar do reconhecedor.
 */
rc_parser GLOBAL_PARSER;

/**
 * Estrutura auxiliar do select.
 */
qr_select GLOBAL_SELECT;

/**
 * Estrutura auxiliar do atual 'where'
 */
qr_filter TEMP_FILTER;
int       TEMP_FILTER_POSITION;

void connect(char *nome) {
    int r;
    r = connectDB(nome);
	if (r == SUCCESS) {
        connected.db_name = malloc(sizeof(char)*((strlen(nome)+1)));

        strcpylower(connected.db_name, nome);

        connected.conn_active = 1;
        printf("You are now connected to database \"%s\" as user \"Ibetres\".\n", nome);
    } else {
    	printf("ERROR: Failed to establish connection with database named \"%s\". (Error code: %d)\n", nome, r);
    }
}

void invalidCommand(char *command) {
    printf("ERROR: Invalid command '%s'. Type \"help\" for help.\n", command);
}

void notConnected() {
    printf("ERROR: you are not connected to any database.\n");
}

void setObjName(char **nome) {
    if (GLOBAL_PARSER.mode != 0) {
        GLOBAL_DATA.objName = malloc(sizeof(char)*((strlen(*nome)+1)));

        strcpylower(GLOBAL_DATA.objName, *nome);
        GLOBAL_DATA.objName[strlen(*nome)] = '\0';
        GLOBAL_PARSER.step++;
    } else
        return;
}

void setColumnInsert(char **nome) {
    GLOBAL_DATA.columnName = realloc(GLOBAL_DATA.columnName, (GLOBAL_PARSER.col_count+1)*sizeof(char *));

    GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count] = malloc(sizeof(char)*(strlen(*nome)+1));
    strcpylower(GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count], *nome);
    GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count][strlen(*nome)] = '\0';

    GLOBAL_PARSER.col_count++;
}

void  setValueInsert(char *nome, char type) {
    int i;
    GLOBAL_DATA.values  = realloc(GLOBAL_DATA.values, (GLOBAL_PARSER.val_count+1)*sizeof(char *));
    GLOBAL_DATA.type    = realloc(GLOBAL_DATA.type, (GLOBAL_PARSER.val_count+1)*sizeof(char));

    // Adiciona o valor no vetor de strings
    nome -= sizeof(char);
    if(*nome != '-'){
		if(*(nome-1) == '-' && *nome == ' ')
			*nome = '-'; 
		else
			nome += sizeof(char);
	}
			
    GLOBAL_DATA.values[GLOBAL_PARSER.val_count] = malloc(sizeof(char)*(strlen(nome)+1));
			
    
			if (type == 'I' || type == 'D') {
		
				strcpy(GLOBAL_DATA.values[GLOBAL_PARSER.val_count], nome);
				GLOBAL_DATA.values[GLOBAL_PARSER.val_count][strlen(nome)] = '\0';
				
				nome -= sizeof(char);
				nome =nome-1;
				char numeros[] = "!@#$%&*+=~^:/?{]}[";
				char *pos_atual = strpbrk(nome, numeros);
				if(pos_atual != NULL){
					return;
		 
				}
			}
				
				
	
        if (type == 'S') {
        for (i = 1; i < strlen(nome)-1; i++) {
            GLOBAL_DATA.values[GLOBAL_PARSER.val_count][i-1] = nome[i];
        }
        GLOBAL_DATA.values[GLOBAL_PARSER.val_count][strlen(nome)-2] = '\0';
    }

    GLOBAL_DATA.type[GLOBAL_PARSER.val_count] = type;

    GLOBAL_PARSER.val_count++;
}

void setColumnCreate(char **nome) {
    GLOBAL_DATA.columnName  = realloc(GLOBAL_DATA.columnName, (GLOBAL_PARSER.col_count+1)*sizeof(char *));
    GLOBAL_DATA.attribute   = realloc(GLOBAL_DATA.attribute, (GLOBAL_PARSER.col_count+1)*sizeof(int));
    GLOBAL_DATA.fkColumn    = realloc(GLOBAL_DATA.fkColumn, (GLOBAL_PARSER.col_count+1)*sizeof(char *));
    GLOBAL_DATA.fkTable     = realloc(GLOBAL_DATA.fkTable, (GLOBAL_PARSER.col_count+1)*sizeof(char *));
    GLOBAL_DATA.values      = realloc(GLOBAL_DATA.values, (GLOBAL_PARSER.col_count+1)*sizeof(char *));
    GLOBAL_DATA.type        = realloc(GLOBAL_DATA.type, (GLOBAL_PARSER.col_count+1)*sizeof(char *));

    GLOBAL_DATA.values[GLOBAL_PARSER.col_count] = malloc(sizeof(char));
    GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count] = malloc(sizeof(char));
    GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count] = malloc(sizeof(char));
    GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count] = malloc(sizeof(char)*(strlen(*nome)+1));

    strcpylower(GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count], *nome);

    GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count][strlen(*nome)] = '\0';
    GLOBAL_DATA.type[GLOBAL_PARSER.col_count] = 0;
    GLOBAL_DATA.attribute[GLOBAL_PARSER.col_count] = NPK;

    GLOBAL_PARSER.col_count++;
    GLOBAL_PARSER.step = 2;
}

void setColumnTypeCreate(char type) {
    GLOBAL_DATA.type[GLOBAL_PARSER.col_count-1] = type;
    GLOBAL_PARSER.step++;
}

void setColumnSizeCreate(char *size) {
    GLOBAL_DATA.values[GLOBAL_PARSER.col_count-1] = realloc(GLOBAL_DATA.values[GLOBAL_PARSER.col_count-1], sizeof(char)*(strlen(size)+1));
    strcpy(GLOBAL_DATA.values[GLOBAL_PARSER.col_count-1], size);
    GLOBAL_DATA.values[GLOBAL_PARSER.col_count-1][strlen(size)-1] = '\0';
}

void setColumnPKCreate() {
    GLOBAL_DATA.attribute[GLOBAL_PARSER.col_count-1] = PK;
}

void setColumnFKTableCreate(char **nome) {
    GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count-1] = realloc(GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count-1], sizeof(char)*(strlen(*nome)+1));
    strcpylower(GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count-1], *nome);
    GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count-1][strlen(*nome)] = '\0';
    GLOBAL_DATA.attribute[GLOBAL_PARSER.col_count-1] = FK;
    GLOBAL_PARSER.step++;
}

void setColumnFKColumnCreate(char **nome) {
    GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count-1] = realloc(GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count-1], sizeof(char)*(strlen(*nome)+1));
    strcpylower(GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count-1], *nome);
    GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count-1][strlen(*nome)] = '\0';
    GLOBAL_PARSER.step++;
}


void clearGlobalStructs() {
    int i;

    if (GLOBAL_DATA.objName) {
        free(GLOBAL_DATA.objName);
        GLOBAL_DATA.objName = NULL;
    }

    for (i = 0; i < GLOBAL_DATA.N; i++ ) {
        if (GLOBAL_DATA.columnName)
            free(GLOBAL_DATA.columnName[i]);
        if (GLOBAL_DATA.values)
            free(GLOBAL_DATA.values[i]);
        if (GLOBAL_DATA.fkTable)
            free(GLOBAL_DATA.fkTable[i]);
        if (GLOBAL_DATA.fkColumn)
            free(GLOBAL_DATA.fkColumn[i]);
    }

    free(GLOBAL_DATA.columnName);
    GLOBAL_DATA.columnName = NULL;

    free(GLOBAL_DATA.values);
    GLOBAL_DATA.values = NULL;

    free(GLOBAL_DATA.fkTable);
    GLOBAL_DATA.fkTable = NULL;

    free(GLOBAL_DATA.fkColumn);
    GLOBAL_DATA.fkColumn = NULL;

    free(GLOBAL_DATA.type);
    GLOBAL_DATA.type = (char *)malloc(sizeof(char));

    free(GLOBAL_DATA.attribute);
    GLOBAL_DATA.attribute = (int *)malloc(sizeof(int));

    yylex_destroy();

    GLOBAL_DATA.N = 0;

    GLOBAL_PARSER.mode              = 0;
    GLOBAL_PARSER.parentesis        = 0;
    GLOBAL_PARSER.noerror           = 1;
    GLOBAL_PARSER.col_count         = 0;
    GLOBAL_PARSER.val_count         = 0;
    GLOBAL_PARSER.step              = 0;

    // Clear select struct
    for (i = 0; i < GLOBAL_SELECT.nprojection; i++)
      free(GLOBAL_SELECT.projection[i]);

    free(GLOBAL_SELECT.projection);
    GLOBAL_SELECT.projection = NULL;

    /* for (i = 0; i < GLOBAL_SELECT.nfilters; i++) */
    /*   free(GLOBAL_SELECT.filters[i]); */

    /* free(GLOBAL_SELECT.filters); */
    /* GLOBAL_SELECT.filters = NULL; */
  
    /* for (i = 0; i < GLOBAL_SELECT.njoins; i++) */
    /*   free(GLOBAL_SELECT.join[i]); */

    /* free(GLOBAL_SELECT.join); */
    /* GLOBAL_SELECT.join = NULL; */
}

void setMode(char mode) {
    GLOBAL_PARSER.mode = mode;
    GLOBAL_PARSER.step++;
}


int interface() {
    pthread_t pth;

    pthread_create(&pth, NULL, (void*)clearGlobalStructs, NULL);
    pthread_join(pth, NULL);

    connect("ibetres"); // conecta automaticamente no banco padrão

    while(1){
        if (!connected.conn_active) {
            printf(">");
        } else {
            printf("%s=# ", connected.db_name);
        }

        pthread_create(&pth, NULL, (void*)yyparse, &GLOBAL_PARSER);
        pthread_join(pth, NULL);

        if (GLOBAL_PARSER.noerror) {
            if (GLOBAL_PARSER.mode != 0) {
                if (!connected.conn_active) {
                    notConnected();
                } else {
                    switch(GLOBAL_PARSER.mode) {
                        case OP_INSERT:
                            if (GLOBAL_DATA.N > 0) {
                                insert(&GLOBAL_DATA);
                            }
                            else
                                printf("WARNING: Nothing to be inserted. Command ignored.\n");
                            break;
                        case OP_SELECT_ALL:
                            imprime(GLOBAL_DATA.objName);
                            break;
                        case OP_CREATE_TABLE:
                            createTable(&GLOBAL_DATA);
                            break;
                        case OP_CREATE_DATABASE:
                            createDB(GLOBAL_DATA.objName);
                            break;
                        case OP_DROP_TABLE:
                            excluirTabela(GLOBAL_DATA.objName);
                            break;
                        case OP_DROP_DATABASE:
                            dropDatabase(GLOBAL_DATA.objName);
                            break;
                        default: break;
                    }

                }
            }
        } else {
            GLOBAL_PARSER.consoleFlag = 1;
            switch(GLOBAL_PARSER.mode) {
                case OP_CREATE_DATABASE:
                case OP_DROP_DATABASE:
                case OP_CREATE_TABLE:
                case OP_DROP_TABLE:
		  // case OP_SELECT_ALL:
                case OP_INSERT:
                    if (GLOBAL_PARSER.step == 1) {
                        GLOBAL_PARSER.consoleFlag = 0;
                        printf("Expected object name.\n");
                    }
                break;

                default: break;
            }

            if (GLOBAL_PARSER.mode == OP_CREATE_TABLE) {
                if (GLOBAL_PARSER.step == 2) {
                    printf("Column not specified correctly.\n");
                    GLOBAL_PARSER.consoleFlag = 0;
                }
            } else if (GLOBAL_PARSER.mode == OP_INSERT) {
                if (GLOBAL_PARSER.step == 2) {
                    printf("Expected token \"VALUES\" after object name.\n");
                    GLOBAL_PARSER.consoleFlag = 0;
                }
            }

            printf("ERROR: syntax error.\n");
            GLOBAL_PARSER.noerror = 1;
        }

        if (GLOBAL_PARSER.mode != 0) {
            pthread_create(&pth, NULL, (void*)clearGlobalStructs, NULL);
            pthread_join(pth, NULL);
        }
    }
    return 0;
}

void yyerror(char *s, ...) {
    GLOBAL_PARSER.noerror = 0;
    /*extern yylineno;

    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    */
}

/**
 * SELECT
 */

void start_select()
{
  GLOBAL_SELECT.nprojection = 0;
  free(GLOBAL_SELECT.projection);
  GLOBAL_SELECT.projection = NULL;

  GLOBAL_SELECT.ntables = 0;
  free(GLOBAL_SELECT.tables);
  GLOBAL_SELECT.tables = NULL;

  GLOBAL_SELECT.nfilters = 0;
  free(GLOBAL_SELECT.filters);
  GLOBAL_SELECT.filters = NULL;

  GLOBAL_SELECT.njoins = 0;
  free(GLOBAL_SELECT.join);
  GLOBAL_SELECT.join = NULL;
}

int add_column_to_projection(char **col_name)
{
  int projIndex;
  char **temp = NULL;

  projIndex = GLOBAL_SELECT.nprojection++; // Atualizando indice da projecao
  temp      = realloc(GLOBAL_SELECT.projection, sizeof(char **) * GLOBAL_SELECT.nprojection);

  // Confere realloc
  if (temp == NULL) {
    fprintf(stderr, "Can't realloc column projection!");
    GLOBAL_PARSER.noerror = 0;
    
    return 0;
  }

  GLOBAL_SELECT.projection = temp; // Sucesso
  GLOBAL_SELECT.projection[projIndex] = malloc(sizeof(char) * (strlen(*col_name) + 1));
  
  // Copia nome da coluna adicionando \0
  strcpylower(GLOBAL_SELECT.projection[projIndex], *col_name);
  GLOBAL_SELECT.projection[projIndex][strlen(*col_name)] = '\0';

  return 1;
}

int create_new_filter()
{
  printf("creating new filter\n");
  TEMP_FILTER.typeLogico = 'N';
  free(TEMP_FILTER.left);
  TEMP_FILTER.left_type = 'N';
  TEMP_FILTER.typeOp = '=';
  free(TEMP_FILTER.right);
  TEMP_FILTER.right_type = 'N';

  return 1;
}

int set_filter_value_pos(int position)
{
  printf("setting to pos %d\n", position);
  if (position == FILTER_POS_LEFT)
    TEMP_FILTER_POSITION = position;
  else
    TEMP_FILTER_POSITION = FILTER_POS_RIGHT;

  return 1;
}

int set_filter_op(char **op)
{
  printf("operation: %s", *op);

  return 1;
}

int add_filter_condition(char **name, int type)
{
  printf("added filter: %s: ", *name);
  
  switch (type) {
  case FILTER_VALUE:
    printf("value\n");
    break;
    
  case FILTER_COLUMN:
    printf("column\n");
    break;
    
  case FILTER_ALPHANUM:
    printf("alphanum\n");
    break;

  case FILTER_NUMBER:
    printf("number\n");
    break;

  default: printf("BUG!\n");
  }

  return 1;
}

int add_filter_to_select();
