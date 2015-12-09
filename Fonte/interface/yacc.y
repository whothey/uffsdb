%{
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "../buffend.h"
#include "parser.h"

extern char* yytext[];
extern FILE * yyin;
extern FILE* outFile_p;

int yywrap() {
    return 1;
}

%}

%union {
    int intval;
    double floatval;
    int subtok;
    char *strval;
}

%%

%token  INSERT      INTO        VALUES      SELECT      FROM
        CREATE      TABLE       INTEGER     VARCHAR     DOUBLE
        CHAR        PRIMARY     KEY         REFERENCES  DATABASE
        DROP        OBJECT      NUMBER      VALUE       QUIT
        LIST_TABLES LIST_TABLE  ALPHANUM    CONNECT     HELP
        LIST_DBASES CLEAR WHERE IN AND OR EXISTS ALL_COLUMNS
        COMP_OP     NATURAL     LEFT        RIGHT       FULL
        JOIN	    ON;


start: insert | select | create_table | create_database | drop_table | drop_database
     | table_attr | list_tables | connection | exit_program | semicolon {GLOBAL_PARSER.consoleFlag = 1; return 0;}
     | parentesis_open | parentesis_close| help_pls | list_databases | clear
     | qualquer_coisa | /*nothing*/;

/*--------------------------------------------------*/
/**************** GENERAL FUNCTIONS *****************/
/*--------------------------------------------------*/

/* CONNECTION */
connection: CONNECT OBJECT {connect(*yytext); GLOBAL_PARSER.consoleFlag = 1; return 0;};

qualquer_coisa: OBJECT {GLOBAL_PARSER.consoleFlag = 1; GLOBAL_PARSER.noerror = 0; return 0;};

/* EXIT */
exit_program: QUIT {exit(0);};

clear: CLEAR {clear(); GLOBAL_PARSER.consoleFlag = 1; return 0;};

parentesis_open: '(' {GLOBAL_PARSER.parentesis++;};

parentesis_close: ')' {GLOBAL_PARSER.parentesis--;};

/* TABLE ATTRIBUTES */
table_attr: LIST_TABLE OBJECT {
    if(connected.conn_active) {
        printTable(yylval.strval);
        GLOBAL_PARSER.consoleFlag = 1;
    } else
        notConnected();
    return 0;
};

/* LIST TABLES */
list_tables: LIST_TABLES {
    if(connected.conn_active) {
        printTable(NULL);
        GLOBAL_PARSER.consoleFlag = 1;
    } else
        notConnected();
    return 0;
};

/* LIST DATABASES */
list_databases: LIST_DBASES {
    showDB();
    GLOBAL_PARSER.consoleFlag = 1;
    return 0;
}

/* HELP */
help_pls: HELP {help(); GLOBAL_PARSER.consoleFlag = 1; return 0;}

/*--------------------------------------------------*/
/****************** SQL STATEMENTS ******************/
/*--------------------------------------------------*/

/* INSERT */
insert: INSERT INTO {setMode(OP_INSERT);} table opt_column_list VALUES parentesis_open value_list parentesis_close semicolon {
    if (GLOBAL_PARSER.col_count == GLOBAL_PARSER.val_count || GLOBAL_DATA.columnName == NULL)
        GLOBAL_DATA.N = GLOBAL_PARSER.val_count;
    else {
        printf("ERROR: The column counter doesn't match the value counter.\n");
        GLOBAL_PARSER.noerror=0;
    }
    return 0;
};

semicolon: ';';

table: OBJECT {setObjName(yytext);};

opt_column_list: /*optional*/ | parentesis_open column_list parentesis_close;

// INSERT COLUMNS
column_list: column | column ',' column_list;
column: OBJECT {setColumnInsert(yytext);};

value_list: value | value ',' value_list;

value: VALUE {setValueInsert(yylval.strval, 'D');}
     | NUMBER {setValueInsert(yylval.strval, 'I');}
     | ALPHANUM {setValueInsert(yylval.strval, 'S');};

/* CREATE TABLE */
create_table: CREATE TABLE {setMode(OP_CREATE_TABLE);} table parentesis_open table_column_attr parentesis_close semicolon {
    GLOBAL_DATA.N = GLOBAL_PARSER.col_count;

    return 0;
};

table_column_attr: column_create type attribute | column_create type attribute ',' table_column_attr;

type: INTEGER {setColumnTypeCreate('I');}
    | VARCHAR {setColumnTypeCreate('S');} parentesis_open NUMBER parentesis_close {setColumnSizeCreate(yylval.strval);}
    | DOUBLE {setColumnTypeCreate('D');};
    | CHAR {setColumnTypeCreate('C');};

column_create: OBJECT {setColumnCreate(yytext);};

attribute: /*optional*/
         | PRIMARY KEY {setColumnPKCreate();}
         | REFERENCES table_fk '(' column_fk ')';

table_fk: OBJECT {setColumnFKTableCreate(yytext);};

column_fk: OBJECT {setColumnFKColumnCreate(yytext);};

/* DROP TABLE */
drop_table: DROP TABLE {setMode(OP_DROP_TABLE);} OBJECT {setObjName(yytext);} semicolon  {return 0;};

/* CREATE DATABASE */
create_database: CREATE DATABASE {setMode(OP_CREATE_DATABASE);} OBJECT {setObjName(yytext);} semicolon {return 0;};

/* DROP DATABASE */
drop_database: DROP DATABASE {setMode(OP_DROP_DATABASE);} OBJECT {setObjName(yytext);} semicolon {return 0;};


/*******************************************
 ** INTERPRETAÇÃO DO SELECT               **
 *******************************************/

// Sintaxe do SELECT
select: SELECT {setMode(OP_SELECT_ALL); start_select();} column_list_projection FROM table_select join_cond where_cond semicolon {return 0;};

// Definindo Projeções
column_list_projection: {add_column_to_projection(yytext);} '*' | column_projection | column_projection ',' column_list_projection;
column_projection: OBJECT {add_column_to_projection(yytext);};

// Filtros (WHERE's)
where_cond: /* define que o WHERE é opcional */ | WHERE filter logic_chain;

logic_chain: /* define que uma cadeia de AND ou OR é opcional, mas adiciona o filtro anterior */ {add_filter_to_select();}
           | logic_op {add_filter_to_select();} filter logic_chain
	   ;

// Definindo aqui a sintaxe VALOR OPERAÇÃO VALOR
filter: {create_new_filter(); set_filter_value_pos(FILTER_POS_LEFT);} column_value COMP_OP {set_filter_op(yytext); set_filter_value_pos(FILTER_POS_RIGHT);} column_value;

// Definindo função ao interpretar um AND ou OR
logic_op: AND {set_filter_logic_op(OP_LOGIC_AND);} | OR {set_filter_logic_op(OP_LOGIC_OR);};

// Identifica uma coluna ou valor especifico
column_value: column_specification | filter_value;

// Determina que uma especificaçao de coluna pode ser COLUNA.CAMPO ou apenas CAMPO
column_specification: filter_column | table_column;

// Veja a definição de promote_filter_and_substitute no 'parser.h' para maiores
// informações de como estas linhas funcionam
filter_column: OBJECT {add_filter_condition(yytext, FILTER_COLUMN);};
table_column: filter_column '.' OBJECT {promote_filter_and_substitute(yytext);};

// Identifica valores (alfanumericos, etc..)
filter_value: ALPHANUM {add_filter_condition(yytext, FILTER_ALPHANUM);}
            | NUMBER {add_filter_condition(yytext, FILTER_NUMBER);}
            | VALUE {add_filter_condition(yytext, FILTER_VALUE);};

join_cond:/* condição JOIN é opcional */ {add_join_to_select();}
         | LEFT    JOIN OBJECT {create_new_join(); set_join_table(yytext); set_join_type(JOIN_TYPE_LEFT);}  ON filter {add_filter_to_join();} join_cond
	 | RIGHT   JOIN OBJECT {create_new_join(); set_join_table(yytext); set_join_type(JOIN_TYPE_RIGHT);} ON filter {add_filter_to_join();} join_cond
	 | FULL    JOIN OBJECT {create_new_join(); set_join_table(yytext); set_join_type(JOIN_TYPE_FULL);}  ON filter {add_filter_to_join();} join_cond
         | NATURAL JOIN OBJECT {create_new_join(); set_join_table(yytext); set_join_type(JOIN_TYPE_NATURAL);} join_cond
         |         JOIN OBJECT {create_new_join(); set_join_table(yytext); set_join_type(JOIN_TYPE_INNER);}  ON filter {add_filter_to_join();} join_cond
	 ;

/* Antigo SELECT */
/*select: SELECT {setMode(OP_SELECT);} column_list_select FROM table_select semicolon {return 0;};*/

table_select: OBJECT {setObjName(yytext); set_select_table(yytext);};

/* END */
%%
