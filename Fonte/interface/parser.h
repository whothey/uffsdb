#include <stdlib.h>
#include <pthread.h>
#include "../buffend.h"
#include <string.h>

/* FUNÇÕES AUXILIARES DO INTERPRETADOR DE COMANDOS (yacc + lex)
 * O yacc vai chamando essas funções conforme vai identificando os tokens.
 * Essas funcções irão preencher uma estrutura, ou parte dela, e chamarão
 * as funções do banco de dados para executar as operações.
 */
#define OP_INSERT 		1
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

/**
 * Select aux
 */
extern qr_select GLOBAL_SELECT; // Current select scope
extern qr_filter *TEMP_FILTER; // Temp filter creation
extern qr_join   *TEMP_JOIN; // Temp join to creation
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


/***********************************************************************
 * SELECT
 *
 * Funções para auxilio da implementação do Select, como o YaCC
 * funciona de token em token (ou pelo menos assim que foi tratado),
 * precisamos de estruturas e variaveis auxiliares, geralmente
 * globais, para manter as informações.  
 *
 * Neste caso as estruturas e variaveis utilizadas são:
 *
 * A estrutura GLOBAL_SELECT, que contém os dados gerais do Select
 * (tais como projeção, quantidade de wheres, estruturas dos wheres,
 * etc...)  e é utilizada pela função de interpretação "doSelect";
 *
 * A estrutura TEMP_FILTER, que representa o filtro que esta sendo
 * construído naquele momento, para futura agregação na estrutura do
 * SELECT;
 * 
 * A variavel TEMP_FILTER_POSITION, que representa a posição do
 * próximo parâmetro, que futuramente deverá ser analisada, já que é
 * possivel comparar as posições já atribuídas para um filtro através
 * do próprio atributo.
 **********************************************************************/


/**
 * Limpa as aspas de uma string (de um valor alfanumerico definido no
 * select)
 */
char *clean_qmarks(char *name);

/** 
 * Inicia o estado geral da estrutura GLOBAL_SELECT
 */
void start_select();

/** 
 * Adiciona uma coluna á projeção do GLOBAL_SELECT, geralmente o valor
 * de resposta do YaCC (yytext).
 */
int add_column_to_projection(char **column);

/**
 * Vincula uma tabela ao select
 *
 * ATENÇÃO: apesar de em alguns SGDBs é possível produzir um produto
 * cartesiano através de "SELECT * FROM table1, table2;", no momento
 * de desenvolvimento desta função, por mais que a variavel da
 * estrutura qr_select esteja no plural, esta funciona só adiciona 1
 * (UMA) tabela ao select que está sendo construído.
 */
int set_select_table(char** table);

/**
 * Cria um novo filtro (alocação e instanciação básica) na variavel
 * auxiliar da estrutura qr_filter (TEMP_FILTER).  Por segurança, o
 * conteúdo anterior da variavel é liberado (free), mas lembrando que
 * este valor deve ser NULL devido á inserção do filtro anteriormente
 * alocado nesta variável na estrutura GLOBAL_SELECT.
 */
int create_new_filter();

/**
 * Seleciona a posição do próximo operador na estrutura temporária de
 * filtro (qr_filter).
 *
 * Como já foi comentado (no inicio das descrições sobre as funções de
 * interpretação do select), uma medida temporária e passível de
 * remoção.
 */
int set_filter_value_pos(int position);

/**
 * Atribui á estrutura de filtro temporária (TEMP_FILTER) a operação
 * presente no parâmetro.
 */
int set_filter_op(char **op);

/**
 * Devido á lógica do YaCC, ou falta de lógica minha, em um
 * determinado momento não pude fazer com que o YaCC interpretasse a
 * regra TABELA.COLUNA seguindo a logica de OBJECT.OBJECT, então tive
 * que pegar o primeiro valor de OBJECT (OBJECT1), inserir como a
 * COLUNA, se o token seguinte for um '.', então promovo essa antiga
 * coluna em tabela, no caso a seguinte sequencia foi digitada
 * (OBJECT1.OBJECT2), e o OBJECT1 estava sendo considerado como
 * coluna, então promovo OBJECT1 para a posição de TABELA na STRUCT
 * TEMP_FILTER e substituo o valor do operando, antes ocupado por
 * OBJECT1 por OBJECT2, que é nosso parametro dessa função.
 */
int promote_filter_and_substitute(char **table);

/**
 * Adiciona um operando no TEMP_FILTER, também se descreve o tipo
 * desse operando;
 *
 * É adicionado na posição definida pela variavel global
 * TEMP_FILTER_POSITION.
 */
int add_filter_condition(char **name, char type);

/**
 * Atribui á estrutura do select a operação lógica (AND ou OR)         --- "ou OR" rsrsrssrsrsrsrsrsrrrsrs
 */
int set_filter_logic_op(char op);

/**
 * Finaliza o filtro atual, remove-o e atribui NULL para a variável
 * temporária, e adiciona-o na lista de estruturas de filtro do
 * SELECT.
 */
int add_filter_to_select();

/******************************
 ** JOIN CLAUSE
 ******************************/
/**
 * Cria uma nova instância de join na variavel global TEMP_JOIN
 */
void create_new_join();

/**
 * Define NATURAL JOIN para a tabela passada como argumento
 */
int set_join_type(int type);

/**
 * Após a definição de um filtro, adiciona-o para a estrutura de JOIN
 */
int add_filter_to_join();

/**
 * Após a finalização da estrutura do JOIN, adiciona-o para a estrutura do SELECT
 */
int add_join_to_select();

/**
 * Define uma tabela para a estrutura de JOIN
 */
int set_join_table(char **table);

/*************************************************************
 ** Funções auxiliares para debug das estruturas globais
 *************************************************************/

/**
 * Informa os dados da estrutura SELECT
 */
void dump_select();

/**
 * Informa os dados da estrutura qr_filter referenciada.
 */
void dump_where(qr_filter filter);

/**
 * Informa os dados da estrutura qr_join referenciada
 */
void dump_join(qr_join join);
