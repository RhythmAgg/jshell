#include "headers.h"

char* tab(char *cmd)
{
    char* hehe = (char*)malloc(sizeof(char)*1024);
    strcpy(hehe,cmd);
    char *tokens[10];
    char *tok = strtok(cmd, " \t\r\n");
    tokens[0] = tok;
    if (tokens[0] == NULL)
    {
        printf("\n");
        struct dirent** d;
        int n = scandir(curr_dir,&d,0,alphasort);
        for(int i=0;i<n;i++){
            printf("%s\n",d[i]->d_name);
        }
        return "";
    }
    //   print_all_ls();
    int count = 0;
    tok = strtok(NULL, " \t\r\n");
    while (tok != NULL)
    {
        count++;
        tokens[count] = tok;
        // printf("%s\n",semicolon_token[cmd_num]);
        // cmd_num++;
        tok = strtok(NULL, " \t\r\n");
    }
    count++;
    char sub[1024];
    strcpy(sub, tokens[count - 1]);
    // printf("\n%s",sub);
    // struct dirent** d;
    // int n = scandir(".",&d,0,alphasort);
    // for(int i=0;i<n;i++){
    //     struct stat filetype;
    //     stat(d[i]->d_name,&filetype);
    //     printf("%s\n",d[i]->d_name);
    //     free(d[i]);
    // }
    char path[1024];
    memset(path,1024,'\0');
    char fname[300];
    int path_ind=strlen(curr_dir);
    if(sub[0] != '.'){
        strcpy(path,curr_dir);
        strcat(path,"/");
        strcat(path,&sub[0]);
    }
    else if(sub[0] == '.' && sub[1] == '/')
    {
        strcpy(path,curr_dir);
        strcat(path,&sub[1]);
    }
    else if(sub[0] == '~' && sub[1] == '/')
    {
        strcpy(path,home_dir);
        strcat(path,&sub[1]);
    }
    for(int i = strlen(path)-1;i>=0;i--)
    {
        if(path[i] == '/')
        {
            strcpy(fname,&path[i+1]);
            for(int j=i+1;j<strlen(path);j++) path[j] = '\0';
            break;
        }
    }
    // printf("\n%s %s\n",path,fname);
    if(chdir(path) !=0){
        perror("Error: ");
        return "";
    }
    char* suggestion[100];
    int index=0;
    struct dirent** d;
    int n = scandir(path,&d,0,alphasort);
    for(int i=0;i<n;i++){
        if(strncmp(d[i]->d_name,fname,strlen(fname)) == 0)
        {
            struct stat filetype;
            stat(d[i]->d_name,&filetype);
            suggestion[index] = (char*)malloc(sizeof(char)*300);
            if(d[i]->d_type ==  4){
                strcpy(suggestion[index],d[i]->d_name);
                strcat(suggestion[index++],"/");
            }
            else strcpy(suggestion[index++],d[i]->d_name);
        }
    }
    // for(int i=0;i<index;i++) printf("%s\n",suggestion[i]);
    if(index == 1){
        int x = strlen(fname);
        int back_index = strlen(hehe)-1;
        while(x--)
        {
            hehe[back_index--] = '\0';
        }
        strcat(hehe,suggestion[0]);
        return hehe;
    }
    else{
        char suggest[100];
        memset(suggest,'\0',100);
        printf("\n");
        for(int i=0;i<index;i++){
            printf("%s\n",suggestion[i]);
        }
        for(int i=0;i<strlen(suggestion[0]);i++){
            int flag=0;
            for(int j=1;j<index;j++){
                if(strlen(suggestion[j]) <= i || strlen(suggestion[j-1]) <= i){
                    flag=1;
                    break;
                }
                if(suggestion[j][i] != suggestion[j-1][i]){
                    flag = 1;
                    break;
                }
            }
            if(flag){
                break;
            }
            suggest[i] = suggestion[0][i];
        }
        int x = strlen(fname);
        int back_index = strlen(hehe)-1;
        while(x--)
        {
            hehe[back_index--] = '\0';
        }
        strcat(hehe,suggest);
        return hehe;
    }
}