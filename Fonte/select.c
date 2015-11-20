#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//No buffend.h
typedef struct list_value{
	char	typeValue;
	char	typeOp;
	char	typeLogic;
	int		ivalue[1];
	double	dvalue[1];
	char	*sname[2];
	struct list_value *next;
}list_value;


int doOperation(const void * a, const void * b, char op, char type);
//@  <=
//#  >=
int select(list_value *value); //, list_op *op, list_logic *logic);


int main()
{
	// 1 < 2 AND abacate != abacate AND 4 < 6
	//Preencher os atributos para fazer 1 consulta. TESTAR

	list_value* pv1 = (list_value *) malloc(sizeof(list_value));
	pv1->next = NULL;

	pv1->typeValue = 'I';
	pv1->typeOp    = '<';
	pv1->typeLogic = 'A';
	pv1->ivalue[0] = 1;
	pv1->ivalue[1] = 2;

	list_value* pv2 = (list_value *) malloc(sizeof(list_value));

	pv2->dvalue[0] = 4;
	pv2->dvalue[1] = 6;
	pv2->typeValue = 'D';
	pv2->typeOp    = '<';
	pv2->typeLogic = 'N';

	pv2->next = NULL;

	list_value* pv3 = (list_value *) malloc(sizeof(list_value));

	pv3->sname[0]  = "Abacate";
	pv3->sname[1]  = "Abacate";
	pv3->typeValue = 'C';
	pv3->typeOp    = '!';
	pv3->typeLogic = 'A';
	pv2->next = NULL;
	pv3->next = pv2;
	pv1->next = pv3;

   // printf("sname0: %s\nsname1: %s\n", pv3->sname[0], pv3->sname[1]);
	//printf("Pv0 = %d\nPv1 = %d\nPv2 = %d\n", pv1->next, pv2,pv3->next);

    int i = select(pv1);
    printf("\nResultado: %d\n", i);
	return 0;

}


int select(list_value *value){//, list_op *op, list_logic *logic){
	int b;

	if(value->typeLogic == 'N'){
		if(value->typeValue == 'D')
			return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue); //Preciso mandar pra função o PONTEIRO, e lá executo conforme necessário.

		if(value->typeValue == 'I')
			return doOperation((int*)&value->ivalue[0], (double*)&value->ivalue[1], value->typeOp, value->typeValue);

		if(value->typeValue == 'C')
			return doOperation((char*)&value->sname[0], (char*)&value->sname[1], value->typeOp, value->typeValue);
	}

	b = select(value->next); //, op->next, logic->next);

	if(value->typeLogic == 'A'){ //Se for AND.
		if(b){ //Se b==1 devo testar o próximo AND
			if(value->typeValue == 'D')
                return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue); ;

			if(value->typeValue == 'I')
				return doOperation((int*)&value->ivalue[0], (double*)&value->ivalue[1], value->typeOp, value->typeValue);

			if(value->typeValue == 'C')
				return doOperation((char*)&value->sname[0], (char*)&value->sname[1], value->typeOp, value->typeValue);
		}
		return 0;
	}
	if(value->typeLogic == 'O'){ //Caso seja OR
		if(b == 0){ //Se for 0 (falso) devo testar o próximo OR
			if(value->typeValue == 'D')
				return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue);

			if(value->typeValue == 'I')
				return doOperation((int*)&value->ivalue[0], (double*)&value->ivalue[1], value->typeOp, value->typeValue);

			if(value->typeValue == 'C')
				return doOperation((char*)&value->sname[0], (char*)&value->sname[1], value->typeOp, value->typeValue);
		}
		//Se for verdadeiro já volta 1
		return 1;
	}
	return 2;
}





int doOperation(const void * v1, const void * v2, char op, char type){ //Retorna 1 caso Verdadeiro

    if(type == 'C'){
        int num = strcmp(v1, v2);
        if(op == '='){
            if(num == 0)
                return 1;
            return 0;
        }
        else if(op == '!'){
            if(num != 0)
                return 1;
            return 0;
        }
    }
    else if(type == 'D'){
        double *a = (double *)v1;
        double *b = (double *)v2;
        if(op == '>'){
            if(*a > *b)
                return 1;
            return 0;
        }
        else if(op == '<'){
            if(*a < *b)
                return 1;
            return 0;
        }
        else if(op == '!'){
            if(*a != *b)
                return 1;
            return 0;
        }
        else if(op == '='){
            if(*a == *b)
                return 1;
            return 0;
        }
        else if(op == '@'){
            if(*a <= *b)
                return 1;
            return 0;
        }
        else if(op == '#'){
            if(*a >= *b)
                return 1;
            return 0;
        }
    }
    else{
        int *a =(int *)v1;
        int *b =(int *)v2;
        if(op == '>'){
            if(*a > *b)
                return 1;
            return 0;
        }
        else if(op == '<'){
            if(*a < *b)
                return 1;
            return 0;
        }
        else if(op == '!'){
            if(*a != *b)
                return 1;
            return 0;
        }
        else if(op == '='){
            if(*a == *b)
                return 1;
            return 0;
        }
        else if(op == '@'){
            if(*a <= *b)
                return 1;
            return 0;
        }
        else if(op == '#'){
            if(*a >= *b)
                return 1;
            return 0;
        }
    }
    return 2;
}




