//BufferPool
#include "buffend.h"


int select(list_value *value){
	int b;

	//Vai parar quando não tiver mais AND ou OR. TypeLogic=N (N de NULL )
	if(value->typeLogic == 'N'){

		if(value->typeValue == 'D')//Caso double, manda o dvalue.
			return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue);

		if(value->typeValue == 'I')//Assim para INT
			return doOperation((int*)&value->ivalue[0], (double*)&value->ivalue[1], value->typeOp, value->typeValue);

		if(value->typeValue == 'C')//E *char
			return doOperation((char*)&value->sname[0], (char*)&value->sname[1], value->typeOp, value->typeValue);

	}
	//Devo fazer cast.. (double *)&value->dvalue[0] .. Porque a próxima função eh constant * void.

	b = select(value->next);

	if(value->typeLogic == 'A'){ //Se for AND.
		//Se b==1 devo testar o próximo AND
		if(b){ 

			if(value->typeValue == 'D')
                return doOperation((double*)&value->dvalue[0], (double*)&value->dvalue[1], value->typeOp, value->typeValue); ;

			if(value->typeValue == 'I')
				return doOperation((int*)&value->ivalue[0], (double*)&value->ivalue[1], value->typeOp, value->typeValue);

			if(value->typeValue == 'C')
				return doOperation((char*)&value->sname[0], (char*)&value->sname[1], value->typeOp, value->typeValue);

		}
		//Caso chegue aqui, já volta 0. Porque deu False
		return 0;
	}
	if(value->typeLogic == 'O'){ //Caso seja OR
		 //Se for 0 (falso) devo testar o próximo OR
		if(b == 0){

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
/*
@  <=
#  >=
<  <
>  >
=  ==
!  !=
*/
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




