//BufferPool
#include "buffend.h"

int verificaNomeTabela(char *nomeTabela) {

    FILE *dicionario;
    char *tupla = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);

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
            free(tupla);
            fclose(dicionario);
            return 1;
        }

        fseek(dicionario, 28, 1);
    }

    fclose(dicionario);
    free(tupla);

    return 0;
}
