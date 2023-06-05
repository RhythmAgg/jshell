#include "headers.h"

int min__(int x,int y)
{
  if(x < y) return x;
  else return y;
}
int compare(const void* a,const void* b)
{
    char x[300];
    char y[300];
    strcpy(x,((bg_process*)a)->process_name);
    strcpy(y,((bg_process*)b)->process_name);
    int l = min__(strlen(x),strlen(y));
    for(int i=0;i<l;i++){
        if(x[i] < y[i]) return -1;
        else if(x[i] > y[i]) return 1;
        else continue;
    }
    if(strlen(x) <  strlen(y)) return -1;
    else if(strlen(x) > strlen(y)) return 1;
    else return 0;
}
void job(char** commands,int cmd_num)
{
    if(cmd_num > 2){
        perror("Error :");
        return;
    }
    int r=0,s=0;
    if(cmd_num == 1) r=s=1;
    else{
        if(strcmp(commands[1],"-r")==0) r=1;
        else if(strcmp(commands[1],"-s")==0) s=1;
        else{
            perror("Error:");
            return;
        }
    }
    bg_process arr[bg_process_size];
    int ii=0;
    bg_process* ptr = head->next;
    while(ptr!=NULL)
    {
        arr[ii++] = (*ptr);
        ptr = ptr->next;
    }
    qsort(arr,bg_process_size,sizeof(bg_process),compare);
    char file_name[1000];
    for(int i=0;i<bg_process_size;i++)
    {
        sprintf(file_name,"/proc/%d/stat",arr[i].pid_process);
        FILE* f = fopen(file_name,"r");
        if(f){
            char status;
            fscanf(f, "%*d %*s %c", &status);
            if(status == 'T'){
                if(s==1){
                    printf("[%d] Stopped %s %d\n",arr[i].process_num,arr[i].process_name,arr[i].pid_process);
                }
            }
            else{
                if(r==1){
                    printf("[%d] Running %s %d\n",arr[i].process_num,arr[i].process_name,arr[i].pid_process);
                }
            }

        }
        // if(r==1){
        //     if(kill(arr[i].pid_process,0)==0){
        //     }
        // }
        // else if(s==1 &&  kill(arr[i].pid_process,0)!=0)
        //     printf("[%d] Stopped %s %d\n",arr[i].process_num,arr[i].process_name,arr[i].pid_process);
        else{
            perror("Error: ");
            return;
        }
        fclose(f);
    }
    // kill(arr[0].pid_process);
}
void sig(char** commands,int cmd_num)
{
    if(cmd_num != 3){
        perror("Error: ");
        return;
    }
    int job_num = atoi(commands[1]);
    int sign = atoi(commands[2]);
    // if(job_num > bg_process_size){
    //     printf("No process with the specified job number exists\n");
    //     return;
    // }
    bg_process* ptr = head->next;
    while(ptr != NULL)
    {
        if(ptr->process_num == job_num)
        {
            if(kill(ptr->pid_process,sign)!=0){
                perror("Error:");
                return;
            }
            return;
        }
    }
    printf("No process with the specified job number exists\n");
}
void fg_(char** commands,int cmd_num)
{
    if(cmd_num != 2){
        perror("Error: ");
        return;
    }
    int job_num = atoi(commands[1]);
    bg_process* prev = head;
    bg_process* ptr = head->next;
    while(ptr != NULL)
    {
        if(ptr->process_num == job_num)
        {
            int pid = ptr->pid_process;
            prev->next = ptr->next;
            if(ptr->next == NULL) tail = prev;
            bg_process_size--;
            int status;
            setpgid(pid, getpgid(0));

            signal(SIGTTIN, SIG_IGN);
            signal(SIGTTOU, SIG_IGN);

            tcsetpgrp(0, pid);

            if (kill(pid, SIGCONT) < 0) perror("Error");

            waitpid(pid, &status, WUNTRACED);

            tcsetpgrp(0, getpgid(0));

            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            return;
        }
        ptr = ptr->next;
        prev = prev->next;
    }
    printf("No process with the specified job number exists\n");
}
void bg_(char** commands,int cmd_num)
{
    if(cmd_num > 2){
        perror("Error: ");
        return;
    }
    int job_num = atoi(commands[1]);
    bg_process* ptr = head->next;
    while(ptr != NULL)
    {
        if(ptr->process_num == job_num)
        {
            if(kill(ptr->pid_process,SIGCONT)<0){
                perror("Error: ");
                return;
            }
            return;
        }
        ptr = ptr->next;
    }
    printf("No process with the job number exists\n");
}