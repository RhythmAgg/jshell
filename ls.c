#include "headers.h"

int file_type(mode_t mode) {
    char c;
    if (S_ISREG(mode))
        c = '-';
    else if (S_ISDIR(mode))
        c = 'd';
    else if (S_ISBLK(mode))
        c = 'b';
    else if (S_ISCHR(mode))
        c = 'c';
    else if (S_ISFIFO(mode))
        c = 'p';
    else if (S_ISLNK(mode))
        c = 'l';
    else if (S_ISSOCK(mode))
        c = 's';
    else {
        c = '?';
    }
    return (c);
}

char* filename(char* path){
    char cwd[400];
    return &path[strlen(strcat(getcwd(cwd,400),"/"))];
}

void ls_path(char* file,int a_flag){
    struct stat check_dir;
    if(stat(file,&check_dir)!=0){
        perror("Error: ");
        return;
    }
    if(S_ISREG(check_dir.st_mode)!=0){
        if(check_dir.st_mode & S_IXUSR) printf("\033[0;32m");
        printf("%s\n",filename(file));
        printf("\033[0m");
        return;
    }
    struct dirent** d;
    int n = scandir(file,&d,0,alphasort);
    for(int i=0;i<n;i++){
        if(a_flag == 0){
            if(d[i]->d_name[0] == '.') continue;
        }
        struct stat filetype;
        stat(d[i]->d_name,&filetype);
        if(d[i]->d_type == 4) printf("\033[0;34m");
        else if((filetype.st_mode & S_IXUSR)) printf("\033[0;32m");
        printf("%s\n",d[i]->d_name);
        printf("\033[0m");
        free(d[i]);
    }
}

void ls_l(char* file,int a_flag){
    struct stat check_dir;
    if(stat(file,&check_dir)!=0){
        perror("Error: ");
        return;
    }
    if(S_ISREG(check_dir.st_mode)!=0){
        ls_l_print(filename(file),0);
        return;
    }
    struct dirent** d;
    int n = scandir(file,&d,0,alphasort);
    int total_blocks=0;
    for(int i=0;i<n;i++){
        if(a_flag == 0){
            if(d[i]->d_name[0] == '.') continue;
        }
        struct stat block;
        stat(d[i]->d_name,&block);
        total_blocks+=block.st_blocks;
    }
    printf("total %d\n",total_blocks);
    for(int i=0;i<n;i++){
        if(a_flag == 0){
            if(d[i]->d_name[0] == '.') continue;
        }
        ls_l_print(d[i]->d_name,0);
    }
}

void ls_l_print(char* file,int a_flag){
    struct stat per;
    if(stat(file,&per)!=0){
        perror("Error: ");
        return;
    }
    char date[30];
    printf("%c",file_type(per.st_mode));
    printf( (per.st_mode & S_IRUSR) ? "r" : "-");
    printf( (per.st_mode & S_IWUSR) ? "w" : "-");
    printf( (per.st_mode & S_IXUSR) ? "x" : "-");
    printf( (per.st_mode & S_IRGRP) ? "r" : "-");
    printf( (per.st_mode & S_IWGRP) ? "w" : "-");
    printf( (per.st_mode & S_IXGRP) ? "x" : "-");
    printf( (per.st_mode & S_IROTH) ? "r" : "-");
    printf( (per.st_mode & S_IWOTH) ? "w" : "-");
    printf( (per.st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");
    printf("%3.hu ",per.st_nlink);
    printf("%s ", getpwuid(per.st_uid)->pw_name);
    printf("%s ", getgrgid(per.st_gid)->gr_name);
    printf("%6.lld ", per.st_size);
    if (time(0) - per.st_mtime < 15780000)
    strftime(date, 20, "%d %b %H:%M", localtime(&(per.st_mtime)));
    else
    strftime(date, 20, "%d %b  %Y", localtime(&(per.st_mtime)));
    printf("%s ",date);
    if(per.st_mode & S_IXUSR) printf("\033[0;32m");
    printf("%s\n",file);
    printf("\033[0m");
}

void ls(char** commands,int cmd_num){
    if(cmd_num == 1){
        ls_path(curr_dir,0);
        return;
    }
    int a_count=0,l_count=0;
    char* fil_dir[cmd_num];
    int index=0;
    for(int i=1;i<cmd_num;i++){
        if(strcmp(commands[i],"-a")==0) a_count++;
        else if(strcmp(commands[i],"-l")==0) l_count++;
        else if(strcmp(commands[i],"-al")==0 || strcmp(commands[i],"-la")==0){
            a_count++;
            l_count++;
        }
        else{
            // fil_dir[index] = (char*)malloc(sizeof(char)*300);
            fil_dir[index++] = commands[i];
        }
    }
    if(l_count==0){
        if(index==0){
            printf(".:\n");
            ls_path(curr_dir,a_count);
        }
        for(int i=0;i<index;i++){
            printf("%s:\n",fil_dir[i]);
            if(fil_dir[i][0] =='.'){
                char temp[400];
                strcpy(temp,curr_dir);
                // printf("%s\n",temp);
                strcat(temp,&fil_dir[i][1]);
                // printf("%s\n",temp);
                chdir(temp);
                ls_path(temp,a_count);
                chdir(curr_dir);
            }
            else if(fil_dir[i][0] == '.' && fil_dir[i][1] == '.'){
                chdir("..");
                char parent[400];
                getcwd(parent,400);
                strcat(parent,&fil_dir[i][2]);
                chdir(parent);
                ls_path(parent,a_count);
                chdir(curr_dir);
            }
            else if(fil_dir[i][0]=='~'){
                char temp[400];
                strcpy(temp,home_dir);
                // printf("%s\n",temp);
                strcat(temp,&fil_dir[i][1]);
                // printf("%s\n",temp);
                chdir(temp);
                ls_path(temp,a_count);
                chdir(curr_dir);
            }
            else{
                if(fil_dir[i][0] == '/'){
                    if(chdir(fil_dir[i])==0){
                        ls_path(fil_dir[i],a_count);
                        chdir(curr_dir);
                    }
                    else{
                        char path[400];
                        strcpy(path,curr_dir);
                        chdir(strcat(path,fil_dir[i]));
                        ls_path(path,a_count);
                        chdir(curr_dir);
                    }
                }
                else{
                    char path[400];
                    strcpy(path,curr_dir);
                    strcat(path,"/");
                    strcat(path,fil_dir[i]);
                    chdir(path);
                    ls_path(path,a_count);
                    chdir(curr_dir);
                }
            }
            if(i != index-1) printf("\n");
        }
    }else{
        if(index==0){
            printf(".:\n");
            ls_l(curr_dir,a_count);
        }
        for(int i=0;i<index;i++){
            printf("%s:\n",fil_dir[i]);
            if(fil_dir[i][0] =='.'){
                char temp[400];
                strcpy(temp,curr_dir);
                // printf("%s\n",temp);
                strcat(temp,&fil_dir[i][1]);
                // printf("%s\n",temp);
                chdir(temp);
                ls_l(temp,a_count);
                chdir(curr_dir);
            }
            else if(fil_dir[i][0] == '.' && fil_dir[i][1] == '.'){
                chdir("..");
                char parent[400];
                getcwd(parent,400);
                strcat(parent,&fil_dir[i][2]);
                chdir(parent);
                ls_l(parent,a_count);
                chdir(curr_dir);
            }
            else if(fil_dir[i][0]=='~'){
                char temp[400];
                strcpy(temp,home_dir);
                // printf("%s\n",temp);
                strcat(temp,&fil_dir[i][1]);
                // printf("%s\n",temp);
                chdir(temp);
                ls_l(temp,a_count);
                chdir(curr_dir);
            }
            else{
                if(fil_dir[i][0] == '/'){
                    if(chdir(fil_dir[i])==0){
                        ls_l(fil_dir[i],a_count);
                        chdir(curr_dir);
                    }
                    else{
                        char path[400];
                        strcpy(path,curr_dir);
                        chdir(strcat(path,fil_dir[i]));
                        ls_l(path,a_count);
                        chdir(curr_dir);
                    }
                }
                else{
                    char path[400];
                    strcpy(path,curr_dir);
                    strcat(path,"/");
                    strcat(path,fil_dir[i]);
                    chdir(path);
                    ls_l(path,a_count);
                    chdir(curr_dir);
                }
            }
            if(i != index-1) printf("\n");
        }
    }
}