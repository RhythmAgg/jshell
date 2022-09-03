#include "headers.h"

void pinfo(char** commands,int cmd_num,pid_t shell_pid){
    pid_t process_id;
    char* status;
    char* memory;
    char* executable = (char*)malloc(sizeof(char*)*300);

    if(cmd_num == 1) process_id = shell_pid;
    else{
        process_id = atoi(commands[1]);
    }
    char stat_process[100], status_process[100], exe_process[100];
    sprintf(stat_process, "/proc/%d/stat", process_id);
    sprintf(status_process, "/proc/%d/status", process_id);
    sprintf(exe_process, "/proc/%d/exe", process_id);
    FILE* stat_open = fopen(stat_process,"r");
    if(stat_open == 0){
        perror("Erro: ");
        return;
    }
    else{
        char* read_stat[25];
        char line[1000];
        char* token = strtok(fgets(line,1000,stat_open)," ");
        read_stat[0] = token;
        token = strtok(NULL," ");
        int index=0;
        while(index < 24){
            index++;
            read_stat[index] = token;
            token = strtok(NULL," ");
        }
        char* status = read_stat[2];
        char* memory = read_stat[22];
        int process_grp = atoi(read_stat[4]);
        int terminal_grp = atoi(read_stat[7]);
        if (process_grp == terminal_grp)
        {
            status[1] = '+';
            status[2] = 0;
        }
        else
        {
            status[1] = 0;
        }
        fclose(stat_open);
    }
    char buffer[1024] = "";
    if(readlink(exe_process,buffer,1024)<=0){
        perror("Error: ");
        return;
    }
    if(strlen(home_dir) > strlen(buffer)){
        strcpy(executable,buffer);
    }
    else{
        if(strncmp(buffer,home_dir,strlen(home_dir)) == 0){
            strcat(executable,"~");
        }
        strcat(executable,&buffer[strlen(home_dir)]);
    }

    printf("pid : %d\n",process_id);
    printf("Process status : %s\n",status);
    printf("Memory : %s\n",memory);
    printf("Executable path : %s\n",executable);
}