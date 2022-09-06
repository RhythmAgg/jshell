#include "headers.h"
void process(char** commands,int cmd_num,int back)
{
    int pid_id = fork();
    commands[cmd_num++] = NULL;
    if(!pid_id)
    {
        setpgid(0,pid_id);
        if(execvp(commands[0],commands)==-1){
            perror("\nError: ");
            exit(0);
        }
        exit(0);
    }
    else if(pid_id && !back)
    {
      int status;
      signal(SIGTTIN, SIG_IGN);
      signal(SIGTTOU, SIG_IGN);
      tcsetpgrp(0, pid_id);
      waitpid(pid_id, &status, WUNTRACED);
      tcsetpgrp(0, getpgid(0));
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
        printf("[%d] %d\n",bg_process_size,node->pid_process);
    }
}