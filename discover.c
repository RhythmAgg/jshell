#include "headers.h"

void traverse_dir(char* dir,int d_flag,int f_flag){
    struct dirent *d;
    DIR* dh = opendir(dir);
    while((d = readdir(dh))!=NULL){
        struct stat filetype;
        stat(d->d_name,&filetype);
        if(S_ISREG(filetype.st_mode)!=0 && f_flag !=0){
            char cwd[400];
            char path[400];
            strcpy(path,strcat(strcat(getcwd(cwd,400),"/"),d->d_name));
            printf(".%s\n",&path[strlen(curr_dir)]);

        }
        if(!S_ISREG(filetype.st_mode) && d_flag!=0 && d->d_name[0] != '.'){
            char cwd[400];
            char path[400];
            strcpy(path,strcat(strcat(getcwd(cwd,400),"/"),d->d_name));
            printf(".%s\n",&path[strlen(curr_dir)]);
            strcpy(cwd,dir);
            char temp[400];
            strcpy(temp,strcat(strcat(cwd,"/"),d->d_name));
            // printf("%s\n",temp);
            if(chdir(temp)!=0){
                perror("Error: ");
                return;
            }
            traverse_dir(temp,d_flag,f_flag);
            chdir("..");
        }
    }
}
int traverse_file(char* dir,char* target,int d_flag,int f_flag){
    struct dirent *d;
    DIR* dh = opendir(dir);
    int flag=0;
    while((d = readdir(dh))!=NULL){
        struct stat filetype;
        stat(d->d_name,&filetype);
        if(S_ISREG(filetype.st_mode)!=0 && f_flag !=0){
            // printf("%s ------ %s\n",d->d_name,target);
            if(strcmp(d->d_name,target)==0){
                char cwd[400];
                char path[400];
                strcpy(path,strcat(strcat(getcwd(cwd,400),"/"),d->d_name));
                printf(".%s\n",&path[strlen(curr_dir)]);
                flag=1;
                break;
            }
        }
        else if(!S_ISREG(filetype.st_mode) && d->d_name[0] != '.'){
            char cwd[400];
            strcpy(cwd,dir);
            char temp[400];
            strcpy(temp,strcat(strcat(cwd,"/"),d->d_name));
            // printf("%s\n",temp);
            if(chdir(temp)!=0){
                perror("Error: ");
                return 0;
            }
            int x = traverse_file(temp,target,d_flag,f_flag);
            chdir("..");
            flag = 0||x;
            if(flag) break;
        }
    }
    return flag;
}

void discover(char** commands,int cmd_num)
{
    if(cmd_num == 1){
        printf(".\n");
        char cwd[400];
        traverse_dir(getcwd(cwd,400),1,1);
        return;
    }
    else{
        int d_flag=0;
        int f_flag=0;
        int target_dir_flag=0;
        int file_to_find_flag=0;
        char target_dir[400];
        char file_to_find[400];
        for(int i=1;i<cmd_num;i++){
            if(strcmp(commands[i],"-d")==0) d_flag=1;
            else if(strcmp(commands[i],"-f")==0) f_flag=1;
            else if(commands[i][0] == '\"'){
                file_to_find_flag=1;
                strcpy(file_to_find,&commands[i][1]);
                file_to_find[strlen(file_to_find)-1] = '\0';
            }
            else{
                target_dir_flag=1;
                strcpy(target_dir,commands[i]);
            }
        }
        if(d_flag ==0 && f_flag ==0){
            d_flag=1;
            f_flag=1;
        }
        if(file_to_find_flag==0){
            if(target_dir_flag==0){
                printf(".\n");
                char cwd[300];
                traverse_dir(getcwd(cwd,300),d_flag,f_flag);
            }else{
                printf("%s\n",target_dir);
                char cwd[300];
                traverse_dir(strcat(strcat(getcwd(cwd,300),"/"),target_dir),d_flag,f_flag);
            }
        }
        else{
            if(target_dir_flag==0){
                char cwd[300];
                if(!traverse_file(getcwd(cwd,300),file_to_find,d_flag,f_flag)){
                    printf("the file doesnt exist\n");
                }   
            }else{
                char cwd[300];
                if(!traverse_file(strcat(strcat(getcwd(cwd,300),"/"),target_dir),file_to_find,d_flag,f_flag)){
                    printf("the file doesnt exist\n");
                }

            }
        }
    }
}