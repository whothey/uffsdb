//BufferPool
#include "buffend.h"

// INSERE NA TABELA
column *insereValor(table  *tab, column *c, char *nomeCampo, char *valorCampo) {
    int i;

	char arrayBigInt[10]; 
	
	memset(arrayBigInt, 0, 10);
	
	strcpy(arrayBigInt, "2147483647"); // Valor maximo do signed int

    column *aux;
    column *e = NULL;

    if(c == NULL){ // Se o valor a ser inserido é o primeiro, adiciona primeiro campo.

        e = (column *)malloc(sizeof(column));

        if (e == NULL)    {
            return ERRO_DE_ALOCACAO;
        }

        memset(e, 0, sizeof(column));

        int tam = retornaTamanhoValorCampo(nomeCampo, tab);
        char tipo = retornaTamanhoTipoDoCampo(nomeCampo,tab);

        int nTam = strlen(valorCampo);

        if (tipo == 'S') {
            nTam = tam;
        }
        
		if (tipo == 'I') {
		
			
			if (nTam > 10) {
				
				printf("WARNING: field exceeded the integer size limit and was not inserted.\n");
				
				return ERRO_INSERIR_VALOR;
				
			}
			
			if (nTam == 10) {		
						
				if (strcmp(valorCampo, arrayBigInt) > 0)	{ // CASO o valor do campo seja maior que o valor MAX do int saira um valor > 0
													
					printf("WARNING: field exceeded the integer size limit and was not inserted.\n");
				
					return ERRO_INSERIR_VALOR;
					
				}					
			}			
		}

        e->valorCampo = (char *)malloc(sizeof(char) * (nTam+1));

        if (e->valorCampo == NULL) {
            free(e);
            return ERRO_DE_ALOCACAO;
        }

        int n = strlen(nomeCampo)+1;

        /**
         * Verifica se o nome ultrapassa o limite, se sim trunca
         */
        if (n > TAMANHO_NOME_CAMPO) {
           n = TAMANHO_NOME_CAMPO;
           printf("WARNING: field name exceeded the size limit and was truncated.\n");
        }


        strncpylower(e->nomeCampo, nomeCampo, n-1);

        n = strlen(valorCampo);

        if (n > tam && tipo == 'S') {
            n = tam;
            printf("WARNING: value of column \"%s\" exceeded the size limit and was truncated.\n", nomeCampo);
        }

        for(i=0; i < n; i++) e->valorCampo[i] = valorCampo[i];
            e->valorCampo[i] = '\0';

        //strncpy(e->valorCampo, valorCampo,n);

        e->next = NULL;
        c = e;
        return c;
    } else {
        for(aux = c; aux != NULL; aux = aux->next) { // Anda até o final da lista de valores a serem inseridos e adiciona um novo valor.
            if(aux->next == NULL){

                e = (column *)malloc(sizeof(column));

                if (e == NULL) {
                    return ERRO_DE_ALOCACAO;
                }

                memset(e, 0, sizeof(column));

                int tam = retornaTamanhoValorCampo(nomeCampo, tab);
                char tipo = retornaTamanhoTipoDoCampo(nomeCampo,tab);

                int nTam = strlen(valorCampo);

                if (tipo == 'S') {
                    nTam = tam;
                }
                
				if (tipo == 'I') {
				
					
					if (nTam > 10) {
						
						printf("WARNING: field exceeded the integer size limit and was not inserted.\n");
						
						return ERRO_INSERIR_VALOR;		
						
					}
					
					if (nTam == 10) {		
								
						if (strcmp(valorCampo, arrayBigInt) > 0)	{	// CASO o valor do campo seja maior que o valor MAX do int saira um valor > 0			
															
							printf("WARNING: field exceeded the integer size limit and was not inserted.\n");
						
							return ERRO_INSERIR_VALOR;
							
						}					
					}		
				}

                e->valorCampo = (char *) malloc (sizeof(char) * (nTam+1));

                if (e->valorCampo == NULL) {
                    free(e);
                    return ERRO_DE_ALOCACAO;
                }

                e->next = NULL;

                int n = strlen(nomeCampo)+1;

                /**
                 * Verifica se o nome do campo ultrapassa o limite, se sim trunca
                 */
                if (n > TAMANHO_NOME_CAMPO) {
                   n = TAMANHO_NOME_CAMPO;
                   printf("WARNING: field name exceeded the size limit and was truncated.\n");
                }

                strncpylower(e->nomeCampo, nomeCampo, n-1);

                //strncpy(e->nomeCampo, nomeCampo,n);

                n = strlen(valorCampo);

                if (n > tam && tipo == 'S') {
                    n = tam;
                    printf("WARNING: value of column \"%s\"exceeded the size limit and was truncated.\n", nomeCampo);
                }

                for(i=0; i < n; i++) e->valorCampo[i] = valorCampo[i];
                e->valorCampo[i] = '\0';

                //strncpy(e->valorCampo, valorCampo,n);
                aux->next = e;
                return c;
            }
        }
    }

    if (e) free(e);
    return ERRO_INSERIR_VALOR;
}
