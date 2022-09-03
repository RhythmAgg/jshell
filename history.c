#include "headers.h"

void history_(){
    int index = (his_index+10) % 20;
    for(int i=0;i<10;i++){
        printf("%s\n",history[index++]);
        index = (index+20) % 20;
    }
}
int history_add(char* cmd){
    // printf("%s\n",cmd);
    // printf("%s\n",history[(his_index+19) % 20]);
    if(strcmp(history[(his_index+19) % 20],cmd)==0) return 0;
    return 1;
}