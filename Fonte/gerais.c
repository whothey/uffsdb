//BufferPool
#include "buffend.h"

// FUNCOES AUXILIARES
int tamTupla(tp_table *esquema, struct fs_objects objeto) {// Retorna o tamanho total da tupla da tabela.

    int qtdCampos = objeto.qtdCampos, i, tamanhoGeral = 0;

    if(qtdCampos){ // Lê o primeiro inteiro que representa a quantidade de campos da tabela.
        for(i = 0; i < qtdCampos; i++)
            tamanhoGeral += esquema[i].tam; // Soma os tamanhos de cada campo da tabela.
    }

    return tamanhoGeral;
}

int pot10(int n) {
    if(n == 0)
        return 1;
    return 10 * pot10(n-1);
}

int strtam(char n[]) {
    if(n[0]==0)
        return 0;
    return 1+strtam(n+1);
}

int convertI(char u[]) {
    if(strtam(u) == 0)
        return 0;
    return (u[0]-48)*pot10(strtam(u)-1) + convertI(u+1);
}

double get_decimal(char u[]) {
    double decimal=0;
    int i,tamanho = strtam(u);
    for(i=0;i<tamanho && u[i]!='.';i++); // posiciona o indice i encima do ponto
    if (u[i] != '.') return 0;
    decimal=convertI(u+i+1);///(pot10(tamanho-i-1));
    decimal=(double)convertI(u+i+1)/(double)(pot10(tamanho-i-1));
    return decimal;
}

double get_inteiro(char v[]) {
    double inteiro=0;
    int i,tamanho = strtam(v);
    char u[10];
    strcpy(u,v); //copia o vetor para uma vaiável auxiliar para preservar sua originalidade
    for(i=0;i<tamanho && u[i]!='.';i++); // posiciona o indice i encima do ponto
    u[i]=0; // separa a parte inteira da parte decimal, inserindo um null no lugar do ponto
    inteiro=convertI(u);
    return inteiro;
}

double convertD(char u[]) {
    return get_inteiro(u)+get_decimal(u);
    //Soma inteiro com decimal.ss
}

double doubAbs(double n){
	if(n<0)
		return n*-1;
	else
		return n;
}

int compDtoD(double a, double b){
	//float e = 0.000000001;
	//printf("a-b=%lf\n",a-b); 
	//if (abs(a - b) <e)
	//printf("X:%d from %lf\n", (int)(abs(a-b)*100000000), doubAbs(a-b));
	if(((int)(doubAbs(a-b)*100000000))==0)
		return 0;
	else	
		return 1;
	
}
