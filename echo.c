#include "headers.h"

void print_echo(char** commands,int cmd_num){
    for(int i=1;i<cmd_num;i++){
        printf("%s ",commands[i]);
    }
    printf("\n");
}