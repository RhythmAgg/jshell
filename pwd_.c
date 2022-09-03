#include "headers.h"

void pwd_(){
    char pwd_dir[1000];
    printf("%s\n",getcwd(pwd_dir,1000));
}