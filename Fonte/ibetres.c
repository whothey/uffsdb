#include <stdlib.h>
#include <string.h>
#include "buffend.h"
#include "interface/y.tab.h"

db_connected connected;

int main(){
    dbInit();
    printf("Ibetres (1.5.15).\nType \"help\" for help.\n\n");
    interface();
    return 1;
}
