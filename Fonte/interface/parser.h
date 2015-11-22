/* FUNÇÕES AUXILIARES DO INTERPRETADOR DE COMANDOS (yacc + lex)
 * O yacc vai chamando essas funções conforme vai identificando os tokens.
 * Essas funcções irão preencher uma estrutura, ou parte dela, e chamarão
 * as funções do banco de dados para executar as operações.
 */
#define OP_INSERT 			1
#define OP_CREATE_TABLE 	2
#define OP_DROP_TABLE 		3
#define OP_CREATE_DATABASE 	4
#define OP_DROP_DATABASE 	5
#define OP_SELECT_ALL 		6
#define OP_SELECT               7

/**
 * FILTERS
 */
// Filter positions
#define FILTER_POS_LEFT  0
#define FILTER_POS_RIGHT 1

// Filter types
#define FILTER_COLUMN   20
#define FILTER_VALUE    30
#define FILTER_ALPHANUM 40
#define FILTER_NUMBER   50

/**
 * qr_filters
 * Guarda os filtros (wheres) á serem executados pela consulta
 *
 * Uma boa maneira de pensar, caso a estrutura tenha ficado confusa é
 * que ela está implementada para SELECTS, exemplo:
 * SELECT * FROM tabela1 WHERE col1  =   2    AND    col2  =  'A' OR col3 = 'B';
 * Posição na estrutura:      (left op right logic   left op right...)
 */
typedef struct qr_filter {
  char typeLogico; // Operador lógico em relação á outras comparações, constantes OP_AND e OP_OR
  char *left;        // Operador da esquerda do filtro (coluna)
  char left_type;    // Tipo do operador da esquerda (coluna ou valor)
  char typeOp;       // Operador lógico da comparação (>, <, =, <>...)
  char *right;       // Operador da direita do filtro
  char right_type;    // Tipo do operador da direita (coluna ou valor)
} qr_filter;

typedef struct qr_join {
  char *table;
  qr_filter condition;
} qr_join;

/**
 * qr_select
 *
 * Estrutura para gerenciar os termos interpretados pelo YaCC e
 * facilitar o acesso em nível da consulta.
 */
typedef struct qr_select {
  char **projection;  // Projeção do select
  int nprojection;
  
  char *tables;       // Tabelas envolvidas no SELECT
  int ntables;
  
  qr_filter *filters; // Filtros (WHEREs)
  int nfilters;
  
  qr_join *join;      // Join -- será implementado posteriormente
  int njoins;
} qr_select;

/**
 * Select aux
 */
extern qr_select GLOBAL_SELECT; // Current select scope
extern qr_filter TEMP_FILTER; // Temp filter creation
extern int       TEMP_FILTER_POSITION;

/* Estrutura global que guarda as informações obtidas pelo yacc
 * na identificação dos tokens
 */
extern rc_insert GLOBAL_DATA;

/* Estrutura auxiliar do reconhecedor.
 */
extern rc_parser GLOBAL_PARSER;

/* Funcções do yacc
 */
int yyparse();
int yylex();
int yylex_destroy();
extern int  yylineno;

/* Função padrão do yacc chamada quando um erro sintático é
 * identificado.
 */
void yyerror(char *s, ...);

/* Imprime o erro caso o comando seja inválido
 */
void invalidCommand(char *command);

/* Mensagem exibida quando o usuário tenta executar um comando
 * sem estar conectado à nenhuma base de dados.
 */
void notConnected();

/* Estabelece a conexão com uma base de dados. Se conectar,
 * define os parâmetros globais para a exibição do console.
 */
void connect(char *nome);


/* Define o nome do banco de dados ou da tabela identificado
 * no token
 */
void setObjName(char **nome);

/* Adiciona na estrutura uma nova coluna identificada no
 * comando INSERT.
 */
void setColumnInsert(char **nome);

/* Define o valor que será inserido, identificado no token
 * do comando INSERT
 */
void setValueInsert(char *nome, char type);

/* Adiciona na estrutura uma nova coluna identificada no
 * comando CREATE TABLE, isso inicializará outras variáveis
 * também (type, attribute, ...)
 */
void setColumnCreate(char **nome);

/* Define o tipo da coluna identificada no comando CREATE
 * TABLE.
 */
void setColumnTypeCreate(char type);

/* Define o tamanho da coluna identificada no comando
 * CREATE TABLE
 */
void setColumnSizeCreate(char *size);

/* Define a coluna identificada no comando CREATE TABLE
 * como PRIMARY KEY
 */
void setColumnPKCreate();

/* Define a tabela que a coluna identificada no CREATE TABLE
 * irá referenciar
 */
void setColumnFKTableCreate(char **nome);

/* Define a coluna da tabela que a coluna identificada
 * no CREATE TABLE irá referenciar.
 */
void setColumnFKColumnCreate(char **nome);

/* Reinicializa todas as propriedades da estrutura que armazena
 * os dados identificados pelos tokens SQL.
 */
void clearGlobalStructs();

/* Define a operação que será realizada pelo identificador através
 * dos tokens iniciais.
 */
void setMode(char mode);


/*********************************************************
 * SELECT
 *********************************************************/
/** 
 * Restarts the global select state
 */
void start_select();

/** 
 * Add an column to select struct
 */
int add_column_to_projection(char **column);

int set_select_table(char* table);

/**
 * Creates a new filter in Global auxiliar struct
 */
int create_new_filter();

/**
 * Set next filter value position in 'filter struct' (qr_filter)
 */
int set_filter_value_pos(int position);

int set_filter_op(char **op);

/**
 * Add filter condition and inform it's type
 */
int add_filter_condition(char **name, int type);

int add_filter_to_select();
