//BufferPool
#include "buffend.h"

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave PK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_PRIMARIA
                ERRO_CHAVE_PRIMARIA_NULA
   ---------------------------------------------------------------------------------------------*/

int verificaChavePK(char *nomeTabela, column *c, char *nomeCampo, char *valorCampo) {
    int j, x, erro, page;
    column *pagina = NULL;

    if(valorCampo[0] == '\0' ){ //verifica se o valor da pk é vazio
        return ERRO_CHAVE_PRIMARIA_NULA;
    }


    struct fs_objects objeto;
    tp_table *tabela;
    tp_buffer *bufferpoll;

    erro = existeAtributo(nomeTabela, c);
    if (erro != SUCCESS ) {
        free(bufferpoll);
        return ERRO_DE_PARAMETRO;
    }


    if (iniciaAtributos(&objeto, &tabela, &bufferpoll, nomeTabela) != SUCCESS) {
        free(bufferpoll);
        free(tabela);
        return ERRO_DE_PARAMETRO;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);

    page = 0;
    for (page = 0; page < PAGES; page++) {
        if (pagina) free(pagina);
        pagina = getPage(bufferpoll, tabela, objeto, page);
        if (!pagina) break;

        for(j = 0; j < objeto.qtdCampos * bufferpoll[page].nrec; j++){
            if (pagina[j].nomeCampo) {
                if (objcmp(pagina[j].nomeCampo, nomeCampo) == 0) {
                    if (pagina[j].tipoCampo == 'S') {
                        if (objcmp(pagina[j].valorCampo, valorCampo) == 0){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'I') {
                        int *n = (int *)&pagina[j].valorCampo[0];

                        if (*n == atoi(valorCampo)) {
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'D'){
                        double *nn = (double *)&pagina[j].valorCampo[0];

                        if (compDtoD((double)*nn, atof(valorCampo))==0){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'C'){
                        if (pagina[j].valorCampo == valorCampo){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    }
                }
            }
        }
    }

    if (pagina) free(pagina);
    free(bufferpoll);
    free(tabela);
    return SUCCESS;
}
