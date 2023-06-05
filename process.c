#include "headers.h"
void process(char** commands,int cmd_num,int back)
{
    commands[cmd_num++] = NULL;
    int pid_id = fork();
    if(pid_id < 0){
        perror("Error: ");
        return;
    }
    if(!pid_id)
    {
        if(back) setpgid(0,0);
        int x = execvp(commands[0],commands);
        if(x==-1){
            perror("Error: ");
            exit(0);
        }
    }
    else if(pid_id && !back)
    {
      int status;
      setpgid(pid_id,getpgid(0));
      signal(SIGTTIN, SIG_IGN);
      signal(SIGTTOU, SIG_IGN);
      strcpy(fg_node->name,commands[0]);
      tcsetpgrp(0, pid_id);
      waitpid(pid_id, &status, WUNTRACED);
      tcsetpgrp(0, getpgid(0));
      if(WIFSTOPPED(status)){
        bg_process* node;
        node = (bg_process*)malloc(sizeof(bg_process));
        strcpy(node->process_name,commands[0]);
        node->pid_process = pid_id;
        node->next = NULL;
        tail->next = node;
        tail = node;
        bg_process_size++;
        if(bg_process_size == 1){
            last_node=0;
        }
        node->process_num = last_node+1;
        last_node++;
        printf("\b\b[%d] %d pushed to the background\n",node->process_num,node->pid_process);
      }
      signal(SIGTTIN, SIG_DFL);
      signal(SIGTTOU, SIG_DFL);
    }
    else
    {
        bg_process* node;
        node = (bg_process*)malloc(sizeof(bg_process));
        strcpy(node->process_name,commands[0]);
        node->pid_process = pid_id;
        node->next = NULL;
        tail->next = node;
        tail = node;
        bg_process_size++;
        if(bg_process_size == 1){
            last_node=0;
        }
        node->process_num = last_node+1;
        last_node++;
        printf("[%d] %d\n",node->process_num,node->pid_process);
    }
}