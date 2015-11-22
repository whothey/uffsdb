#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erros.h"

#define SIZE 512                // Tamanho da página.
#define PAGES 1024              // Número de páginas.
#define TAMANHO_NOME_CAMPO 40   // Tamanho do nome dos campos de uma tabela.
#define TAMANHO_NOME_TABELA 20  // Tamanho do nome da tabela.
#define TAMANHO_NOME_ARQUIVO 20 // Tamanho do nome do arquivo.
#define QTD_DB 100
#define LEN_DB_NAME 20
#define DB_EXISTS 1
#define DB_NOT_EXISTS 24


struct fs_objects { // Estrutura usada para carregar fs_objects.dat
    char nome[TAMANHO_NOME_TABELA];     //  Nome da tabela.
    int cod;                            // Código da tabela.
    char nArquivo[TAMANHO_NOME_ARQUIVO];// Nome do arquivo onde estão armazenados os dados da tabela.
    int qtdCampos;                      // Quantidade de campos da tabela.
};

typedef struct tp_table{ // Estrutura usada para carregar fs_schema.dat
    char nome[TAMANHO_NOME_CAMPO];  // Nome do Campo.                    40bytes
    char tipo;                      // Tipo do Campo.                     1bytes
    int tam;                        // Tamanho do Campo.                  4bytes
    int chave;                      // Tipo da chave                      4bytes
    char tabelaApt[TAMANHO_NOME_TABELA]; //Nome da Tabela Apontada        20bytes
    char attApt[TAMANHO_NOME_CAMPO];    //Nome do Atributo Apontado       40bytes
    struct tp_table *next;          // Encadeamento para o próximo campo.
}tp_table;

typedef struct column{ // Estrutura utilizada para inserir em uma tabela, excluir uma tupla e retornar valores de uma página.
    char tipoCampo;                     // Tipo do Campo.
    char nomeCampo[TAMANHO_NOME_CAMPO]; //Nome do Campo.
    char *valorCampo;                   // Valor do Campo.
    struct column *next;                // Encadeamento para o próximo campo.
}column;

typedef struct table{ // Estrutura utilizada para criar uma tabela.
    char nome[TAMANHO_NOME_TABELA]; // Nome da tabela.
    tp_table *esquema;              // Esquema de campos da tabela.
}table;

typedef struct tp_buffer{ // Estrutura utilizada para armazenar o buffer.
   unsigned char db;        //Dirty bit
   unsigned char pc;        //Pin counter
   unsigned int nrec;       //Número de registros armazenados na página.
   char data[SIZE];         // Dados
   unsigned int position;   // Próxima posição válida na página.
}tp_buffer;

typedef struct rc_insert {
    char    *objName;           // Nome do objeto (tabela, banco de dados, etc...)
    char   **columnName;        // Colunas da tabela
    char   **values;            // Valores da inserção ou tamanho das strings na criação
    int      N;                 // Número de colunas de valores
    char    *type;              // Tipo do dado da inserção ou criação de tabela
    int     *attribute;         // Utilizado na criação (NPK, PK,FK)
    char   **fkTable;           // Recebe o nome da tabela FK
    char   **fkColumn;          // Recebe o nome da coluna FK
}rc_insert;

typedef struct rc_parser {
    int         mode;           // Modo de operação (definido em /interface/parser.h)
    int         parentesis;     // Contador de parenteses abertos
    int         step;           // Passo atual (token)
    int         noerror;        // Nenhum erro encontrado na identificação dos tokens
    int         col_count;      // Contador de colunas
    int         val_count;      // Contador de valores
    int         consoleFlag;   // Auxiliar para não imprimir duas vezes nome=#
}rc_parser;

typedef struct data_base {
	char 		valid;
	char 		db_name[LEN_DB_NAME];
	char 		db_directory[LEN_DB_NAME];
}data_base;

typedef struct db_connected {
	char db_directory[LEN_DB_NAME*2];
    char *db_name;
    int conn_active;
}db_connected;

// Union's utilizados na conversão de variáveis do tipo inteiro e double.

union c_double{

    double dnum;
    char double_cnum[sizeof(double)];
};

union c_int{

    int  num;
    char cnum[sizeof(int)];
};

/************************************************************************************************
**************************************  VARIAVEIS GLOBAIS  **************************************/

extern db_connected connected;

/************************************************************************************************
 ************************************************************************************************/

tp_buffer * initbuffer();
/*
    Esta função tem por objetivo criar e inicializar uma estrutura do tipo tp_buffer
    que será usada para carregar tuplas na memória

*/

/************************************************************************************************
 ************************************************************************************************/

struct fs_objects leObjeto(char *nTabela);
/*
    Esta função busca, no arquivo fs_object.dat, pelo nome da tabela retornando as informações que
    estão no dicionário em uma estrutura fs_objects. Caso o nome da tabela não exista, o programa
    aborta.

    *nTabela - Nome da tabela a ser buscado no dicionário de dados
*/

/************************************************************************************************
 ************************************************************************************************/

tp_table *leSchema (struct fs_objects objeto);
/*
    Esta função busca, no arquivo fs_schema.dat, pelas informações do objeto, carregando o esquema
    da tabela que é retornadado em tp_table.
    Caso o nome da tabela não exista, o programa aborta
    *objeto - Objeto, já previamente inicializado em leObjeto(nTabela), que contém as informações
              sobre uma determinada tabela.
*/

/************************************************************************************************
 ************************************************************************************************/

int tamTupla(tp_table *esquema, struct fs_objects objeto);
/*
    Esta função calcula, usando o esquema e o dicionário de dados, o tamanho da tupla de uma
    tabela, retornando o mesmo.

    *esquema - Estrutura que contém o esquema da tabela (nome de campo, tipo de campo, etc)
    *objeto  - Estrutura que contém informações sobre a tabela (nome da tabela, nome do arquivo da
               tabela, etc)

*/

/************************************************************************************************
 ************************************************************************************************/

int printbufferpoll(tp_buffer *buffpoll, tp_table *s,struct fs_objects objeto, int num_page);
/*
    Esta função imprime todos os dados carregados numa determinada página do buffer
    *buffer - Estrutura para armazenar tuplas na memória
    *s - Estrutura que armazena esquema da tabela para ler os dados do buffer
    *objeto - Estrutura que armazena dados sobre a tabela que está no buffer
    *num_page - Número da página a ser impressa
*/

/************************************************************************************************
 ************************************************************************************************/

int colocaTuplaBuffer(tp_buffer *buffer, int from, tp_table *campos, struct fs_objects objeto);
/*
    Esta função insere uma tupla em uma página do buffer em que haja espaço suficiente.
    Retorna ERRO_BUFFER_CHEIO caso não haja espeço para a tupla

    *buffer - Estrutura para armazenar tuplas na meméria
    *from   - Número da tupla a ser posta no buffer. Este número é relativo a ordem de inserção da
              tupla na tabela em disco.
    *campos - Estrutura que armazena esquema da tabela para ler os dados do buffer
    *objeto - Estrutura que armazena dados sobre a tabela que está no buffer
*/

/************************************************************************************************
 ************************************************************************************************/

int quantidadeTabelas();
/*
    Esta função conta quantas tabelas já estão inseridas dentro do dicionario, para poder colocar
    um código válido para a próxima tabela. Retorna a quantidade exata de tabelas.
*/

/************************************************************************************************
 ************************************************************************************************/

int verificaNomeTabela(char *nomeTabela);
/*
    Esta função verifica se um nome de tabela já está inserido no dicionario.
    Retorna:
        -> 1 se o nome existe no dicionario;
        -> 0 se existe no dicionário.
    *nomeTabela - Nome de uma tabela,  a qual deseja-se saber se existe no dicionario.
*/

/************************************************************************************************
 ************************************************************************************************/

table *iniciaTabela(char *nomeTabela);
/*
    Esta função inicia um estrutura do tipo table, como nome de tabela passado.
    Retorna:
        -> a estrutura do tipo table iniciada;
        -> ERRO_NOME_TABELA_INVALIDO se o nome passado já existir no dicionário.
    *nomeTabela - Nome de uma tabela, a qual deseja-se inserir.
*/

/************************************************************************************************
 ************************************************************************************************/

table *adicionaCampo(table *t,char *nomeCampo, char tipoCampo, int tamanhoCampo, int tChave, char *tabelaApt, char *attApt);
/*
    Esta função encadeia a lista de campos na estrutura de uma tabela que vai ser criada.
    Retorna:
        -> a estrutura com a coluna inserida na lista.
    *t - Estrutura da tabela à ser criada.
    *nomeCampo - Nome do campo que irá ser inserido na lista de campos.
    tipoCampo - Tipo do campo que irá ser inserido na lista de campos.
    tamanhoCampo - Tamanho do campo que irá ser inserido na lista de campos.
*/

/************************************************************************************************
 ************************************************************************************************/

int finalizaTabela(table *t);
/*
    Esta função finaliza a tabela preveamente estrutura pelas funcoes iniciaTabela() e adicionaCampo().
    Escreve nos arquivos fs_object.dat e fs_schema.dat, a estrutura passada.
    Retorna:
        -> SUCCESS quando teve sucesso na sua operaçãoç;
        -> ERRO_ABRIR_ARQUIVO quando teve problemas ao abrir os arquivos fs_object.dat e fs_schema.dat;
        -> ERRO_PARAMETRO quando a estrutura passada é inválida.
    *t - Estrutura da tabela à ser criada.
*/

/************************************************************************************************
 ************************************************************************************************/

column *insereValor(table  *tab, column *c, char *nomeCampo, char *valorCampo);
/*
    Esta função inicia e aloca dinâmicamente uma lista de valores que vão ser inseridos em uma tabela.
    Retorna:
        -> estrutura iniciada e alocad com o valor passado por parâmetro.
        -> ERRO_DE_PARAMETRO, quando a estrutura enviada for inválida.
    *c - Estrutura de valores que vão ser inseridos em uma tabela.
    *nomeCampo - Nome do campo que o usuário vai inserir um valor.
    *valorCampo - Valor do campo que vai ser inserido.
*/

/************************************************************************************************
 ************************************************************************************************/

int finalizaInsert(char *nome, column *c);
/*
    Esta função finaliza a inserção de valores em uma tabela. Assume que o usuário entrou com todos
    os campos de uma tupla completa.
    Retorna:
        -> ERRO_ABRIR_ARQUIVO, quando ocorreu um erro ao abrir o arquivo fs_object.dat ou fs_schema.dat;
        -> ERRO_NO_TAMANHO_STRING, quando ocorreu um erro no tamanho da string inserida;
        -> ERRO_NOME_CAMPO, quando o nome do campo passado na estrutura;
        -> ERRO_NO_TIPO_INTEIRO, quando o valor passado não é um inteiro;
        -> ERRO_NO_TIPO_DOUBLE, quando o valor passado não é um double;
        -> ERRO_NO_TIPO_CHAR, quando o valor passado é maior que 1byte;
        -> SUCCESS, quando a função teve sucesso em sua operação de inserção na tabela.
    *nome - Nome da tabela que vai ser inserido os valores da estrutura *c.
    *c - Estrutura com o valores que vão ser inseridos na tabela *nome.
*/

/************************************************************************************************
 ************************************************************************************************/

column * getPage(tp_buffer *buffer, tp_table *campos, struct fs_objects objeto, int page);

/*
    Esta função recupera uma página do buffer e retorna a mesma em uma estrutura do tipo column
    A estrutura column possui informações de como manipular os dados
    *buffer - Estrutura para armazenar tuplas na meméria
    *campos - Estrutura que armazena esquema da tabela para ler os dados do buffer
    *objeto - Estrutura que armazena dados sobre a tabela que está no buffer
    *page - Número da página a ser recuperada (0 a PAGES)
*/

/************************************************************************************************
 ************************************************************************************************/

column * excluirTuplaBuffer(tp_buffer *buffer, tp_table *campos, struct fs_objects objeto, int page, int nTupla);
/*
    Esta função uma determinada tupla do buffer e retorna a mesma em uma estrutura do tipo column;
    A estrutura column possui informações de como manipular os dados
    *buffer - Estrutura para armazenar tuplas na meméria
    *campos - Estrutura que armazena esquema da tabela para ler os dados do buffer
    *objeto - Estrutura que armazena dados sobre a tabela que está no buffer
    *page   - Número da página a ser recuperada uma tupla (0 a PAGES)
    *nTupla - Número da tupla a ser excluida, este número é relativo a página do buffer e não a
              todos os registros carregados
*/

/************************************************************************************************
/  Natan J. Mai, Ricardo Zanuzzo e Rogério Torchelsen                                          */

void imprime(char nomeTabela[] );
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Utilizada para impressão de tabelas.
    Parametros: Nome da tabela (char).
    Retorno:    void.
   ---------------------------------------------------------------------------------------------*/

int excluirTabela(char *nomeTabela);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Função para exclusão de tabelas.
    Parametros: Nome da tabela (char).
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_OBJECT,
                ERRO_REMOVER_ARQUIVO_SCHEMA,
                ERRO_LEITURA_DADOS.
   ---------------------------------------------------------------------------------------------*/

int existeArquivo(const char* filename);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Verificação de existência de um arquivo.
    Parametros: Nome do arquivo.
    Retorno:    INT 1 (existe) , 0 (não existe).
   ---------------------------------------------------------------------------------------------*/

int existeAtributo(char *nomeTabela, column *c);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Verifica a existência do atributo antes de adicionar na tabela
    Parametros: Nome da tabela, coluna C.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO
   ---------------------------------------------------------------------------------------------*/

//TrocaArquivosObj:
int TrocaArquivosObj(char *nomeTabela, char *linha);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Verifica se o nome da tabela 'nomeTabela' está nos primeiros bytes de 'linha'
    Parametros: Nome da tabela, char linha.
    Retorno:    INT(1 - Está contido, 0 - Não está)
   ---------------------------------------------------------------------------------------------*/

tp_table *procuraAtributoFK(struct fs_objects objeto);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Retorna vetor de esquemas com todos os atributos chaves (PK, FK e NPK)
    Parametros: Objeto da tabela.
    Retorno:    Vetor de esquemas vetEsqm
   ---------------------------------------------------------------------------------------------*/

int procuraObjectArquivo(char *nomeTabela);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Copia todas as informações menos a tabela com nome NomeTabela, que será removida.
    Parametros: Nome da tabela que será removida do object.dat.
    Retorno:    INT
                SUCCESS,
                ERRO_ABRIR_ARQUIVO
   ---------------------------------------------------------------------------------------------*/

//procuraSchemaArquivo:
int procuraSchemaArquivo(struct fs_objects objeto);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Copia todas as informações menos a tabela do objeto, que será removida.
    Parametros: Objeto que será removido do schema.
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_SCHEMA
   ---------------------------------------------------------------------------------------------*/

int verificaChaveFK(char *nomeTabela, column *c, char *nomeCampo, char *valorCampo, char *tabelaApt, char *attApt);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave FK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo, Tabela Apontada e Atributo Apontado.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_ESTRANGEIRA
   ---------------------------------------------------------------------------------------------*/

int verificaChavePK(char *nomeTabela, column *c, char *nomeCampo, char *valorCampo);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave pK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_PRIMARIA
   ---------------------------------------------------------------------------------------------*/

int iniciaAtributos(struct fs_objects *objeto, tp_table **tabela, tp_buffer **bufferpoll, char *nomeT);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Inicializa os atributos necessários para a verificação de FK e PK.
    Parametros: Objeto da tabela, Tabela, Buffer e nome da tabela.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
   ---------------------------------------------------------------------------------------------*/

//função que
tp_table *abreTabela(char *nomeTabela, struct fs_objects *objeto, tp_table **tabela);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Recebe o nome de uma tabela e engloba as funções leObjeto() e leSchema().
    Parametros: Nome da Tabela, Objeto da Tabela e tabela.
    Retorno:    tp_table
   ---------------------------------------------------------------------------------------------*/


int retornaTamanhoValorCampo(char *nomeCampo, table  *tab);

void cria_campo(int tam, int header, char *val, int x);

char *getTupla(tp_table *campos,struct fs_objects objeto, int from);

void setTupla(tp_buffer *buffer,char *tupla, int tam, int pos);

double convertD(char u[]);

char retornaTamanhoTipoDoCampo(char *nomeCampo, table  *tab);

int drawline(tp_buffer *buffpoll, tp_table *s, struct fs_objects objeto, int p, int num_page);

int cabecalho(tp_table *s, int num_reg);

/*------------------------------------------------------------------------------------ */
/* ----------------- Igor Beilner, Eliton Traverssini, Régis T. Feyh ----------------- */
/*------------------------------------------------------------------------------------ */
/* insert: Recebe uma estrutura rc_insert e valida os tokens encontrados pela interface().
 *         Se os valores forem válidos, insere um novo valor.
 */
void insert(rc_insert *nomeTabela);

/* interface: Parte gerada pelo yacc, presente no arquivo yacc.y
 *            É responsável por fazer a conexão ao banco e chamar yyparse()
 *            para fazer a validação dos comandos enviados pelo usuário.
 */
int interface();

/* printTable: Imprime a list de tabelas quando o usuário enviar o comando \d
 *             ou imprime a list de atributos da tabela quando o usuário informar
 *             \dt <Nome da tabela>
 */
void printTable(char *tbl);

/* help: Ajuda do programa
 *
 */
void help();

/* objcmp: Recebe o nome de um objeto e a entrada do usuário e
 *         verifica se são iguais sem diferenciar caracteres
 *         maiúsculos e minúsculos.
 */
int objcmp(char *obj, char *str);

/* strcpylower: Recebe duas strings dest e src,
 *              copia src para dest em minúsculo
 */
void strcpylower(char *dest, char *src);

/* strncpylower: Recebe duas strings dest e src e o tamanho de src,
 *               copia src para dest em minúsculo no tamanho length
 */
void strncpylower(char *dest, char *src, int length);

/* createTable: Recebe uma estrutura contendo os parâmetros do CREATE TABLE
 *              e cria uma tabela no banco corrente
 */
void createTable(rc_insert *t);

/* createDB: Recebe um nome para criar um banco de dados
 *           cria o banco, caso ainda não exista
 */
void createDB(char *db_name);

/* connectDB: Recebe o nome de um banco de dadados
 *            conecta-se com o banco, caso exista
 */
char connectDB(char *db_name);

/* dbInit: cria o banco de dados padrão
 *         com o nome "ibetres", caso não exista
 */
void dbInit();

/* dropDatabase: Recebe o nome de um banco de dados
 *               deleta o banco caso o banco exista
 *               e o usuário esteja conectado em outro banco
 */
void dropDatabase(char *db_name);

/* showDB: lista todos os bancos do SGBD "\l"
 *
 */
void showDB();

/* clear: limpa o shell do SGBD
 *
 */
void clear();

void freeTp_table(tp_table **tabela, int n);
void freeTable(table *tabela);
void freeColumn(column *colunas);

int compDtoD(double a, double b);
/*
*  Essa função é utilizada para comparar duas variáveis double e retorna:
*	0: Se forem iguais
*	1: Se forem diferentes
*  Em virtude da maneira como as variáveis double são representadas, essas variáveis não podem ser comparadas
*  diretamente, sendo necessário atribuir uma margem de erro.
*/

double doubAbs(double n);
/*
* Retorna o valor absoluto de uma variável double;
*/
