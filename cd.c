#include "headers.h"

void cd(char** commands,int cmd_num)
{
    if(cmd_num > 2){
        perror("Error: ");
        return;
    }
    if(cmd_num == 1){
        strcpy(prev_dir,curr_dir);
        strcpy(curr_dir,home_dir);
        return;
    }
    else{
        if(commands[1][0] == '~')
        {
            char path[400];
            strcpy(path,home_dir);
            strcat(path,&commands[1][1]);
            chdir(path);
            strcpy(prev_dir,curr_dir);
            getcwd(curr_dir,300);
        }
        else if(strcmp(commands[1],".")==0){
            strcpy(prev_dir,curr_dir);
            return;
        }
        else if(strcmp(commands[1],"..")==0){
            // if(strcmp(curr_dir,home_dir)==0){
            //     printf("%s\n",curr_dir);
            //     return;
            // }
            chdir(".."); 
            strcpy(prev_dir,curr_dir);
            getcwd(curr_dir,300);
            return;
        }
        else if(strcmp(commands[1],"-")==0){
            chdir(prev_dir);
            strcpy(prev_dir,curr_dir);
            getcwd(curr_dir,300);
            printf("%s\n",curr_dir);
        }
        else{
            strcpy(prev_dir,curr_dir);
            // if(commands[1][0] == '/'){
            //     printf("Error: invalid directory");
            //     return;
            // }
            // strcat(curr_dir,"/");
            // if(chdir(strcat(curr_dir,commands[1]))!=0){
            //     perror("Error: ");
            // }
            // getcwd(curr_dir,300);
            if(commands[1][0] == '/'){
                if(chdir(commands[1])==0){
                    getcwd(curr_dir,300);
                }
                else{
                    char path[400];
                    strcpy(path,curr_dir);
                    if(chdir(strcat(path,commands[1])) !=0){
                        perror("Error: ");
                        return;
                    }
                    getcwd(curr_dir,300);
                }
            }
            else{
                char path[400];
                strcpy(path,curr_dir);
                strcat(path,"/");
                strcat(path,commands[1]);
                if(chdir(path)!=0){
                    perror("Error: ");
                    return;
                }
                getcwd(curr_dir,300);
            }
        }
    }
}