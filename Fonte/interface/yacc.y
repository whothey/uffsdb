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
        COMP_OP;

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

/***      ***
 ** SELECT **
 ***      ***/

// SELECT COLUMNS
column_list_projection: {add_column_to_projection(yytext);} '*' | column_projection | column_projection ',' column_list_projection;
column_projection: OBJECT {add_column_to_projection(yytext);};

// Filters
filter: {create_new_filter(); set_filter_value_pos(FILTER_POS_LEFT);} column_value COMP_OP {set_filter_op(yytext); set_filter_value_pos(FILTER_POS_RIGHT);} column_value;

// {set_filter_op(yytext); set_filter_value_pos(FILTER_POS_RIGHT);}

logic_op: AND {set_filter_logic_op(OP_LOGIC_AND);} | OR {set_filter_logic_op(OP_LOGIC_OR);};

column_value: VALUE  {add_filter_condition(yytext, FILTER_VALUE);}
            | NUMBER {add_filter_condition(yytext, FILTER_NUMBER);}
            | ALPHANUM {add_filter_condition(yytext, FILTER_ALPHANUM);}
            | OBJECT {add_filter_condition(yytext, FILTER_COLUMN);}
            ;

where_cond: /* optional */ | WHERE filter logic_chain;

logic_chain: /* optional */ {add_filter_to_select();} | logic_op {add_filter_to_select();} filter logic_chain;

// Main select
select: SELECT {setMode(OP_SELECT_ALL); start_select();} column_list_projection FROM table_select where_cond semicolon {return 0;};

/* SELECT */
/*select: SELECT {setMode(OP_SELECT);} column_list_select FROM table_select semicolon {return 0;};*/

table_select: OBJECT {setObjName(yytext); set_select_table(yytext);};

/* END */
%%
