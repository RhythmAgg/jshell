#include "headers.h"
 
char home_dir[300];
char prev_dir[300];
char curr_dir[300];
char* history[20];
int his_index=0;
pid_t shell_pid;

const char* getCurrentDir()
{
  char* temp;
  char* output;
  temp = (char*)malloc(sizeof(char)*300);
  output = (char*)malloc(sizeof(char)*300);
  if(strlen(curr_dir) < strlen(home_dir)){
    strcpy(output,curr_dir);
    return output;
  }
  if(strncmp(curr_dir,home_dir,strlen(home_dir))==0){
    strcat(output,"~");
  }
  strcat(output,&curr_dir[strlen(home_dir)]);
  return output;
}
// void prompt(char* str,int* flag){
//   printf("\033[0;32m");
//   if((*flag) == -1) printf("%s%s>",str,getCurrentDir());
//   else{
//     printf("%stook %fs>",str,(*flag));
//     (*flag) = -1;
//   }
//   printf("\033[0m");
// }
void bg_handler(int sig)
{
  printf("\n");
  int status;
  bg_process* ptr;
  bg_process* prev;
  prev = head;
  ptr = head->next;
  int exit_flag=0;
  while(ptr != NULL)
  {
    if(waitpid(ptr->pid_process, &status, WNOHANG) != 0)
    {
      if(WIFEXITED(status)){
        exit_flag = 1;
      }
      printf("%s process with pid %d has exited %s\n",ptr->process_name,ptr->pid_process,(exit_flag) ? "normally" : "abnormally");
      prev->next = ptr->next;
      ptr = ptr->next;
      if(ptr == NULL) tail = prev;
      bg_process_size--;
    }
    else
    {
      prev = prev->next;
      ptr = ptr->next;
    }
  }
  // printf("take commands here now: ");
}

void history_save(int sig)
{
  char history_sahi_jagah[500];
  strcpy(history_sahi_jagah,home_dir);
  strcat(strcat(history_sahi_jagah,"/"),"history_logs.txt");
  int fd = open(history_sahi_jagah,O_WRONLY | O_CREAT | O_TRUNC, 0700);
  for(int i=0;i<20;i++){
    // printf("%s\n",history[i]);
    write(fd,history[i],strlen(history[i]));
    write(fd,"\n",1);
  }
  char write_index[4];
  sprintf(write_index,"%d",his_index);
  write(fd,write_index,strlen(write_index));
  exit(sig);
}

const char *getUserName()
{
  uid_t uid = geteuid();
  struct passwd *pw = getpwuid(uid);
  if (pw)
  {
    return pw->pw_name;
  }

  return "";
}

int main()
{
    signal(SIGCHLD,bg_handler);
    signal(SIGINT,history_save);
    bg_process_size=0;
    head = (bg_process*)malloc(sizeof(bg_process));
    tail = (bg_process*)malloc(sizeof(bg_process));
    head->next = NULL;
    head->pid_process = -1;
    tail = head;
    shell_pid = getpid();
    char hostname[256];
    gethostname(hostname,sizeof(hostname));
    char username[300];
    strcpy(username,getUserName());
    char str[1000]="<";
    getcwd(home_dir,300);
    strcpy(prev_dir,home_dir);
    strcpy(curr_dir,home_dir);
    strcat(strcat(strcat(strcat(str,username),"@"),hostname),":");
    FILE* file_his = fopen("history_logs.txt","r");
    size_t len = 1000;
    for(int i=0;i<20;i++){
      history[i] = (char*)malloc(sizeof(char)*1000);
      fgets(history[i],1000,file_his);
      history[i][strlen(history[i]) - 1] = '\0';
      // printf("ok\n");
      // printf("%s\n",history[i]);
    }
    // for(int i=0;i<20;i++) printf("%s\n",history[i]);

    char* read_index = (char*)malloc(sizeof(char)*4);
    read_index = strtok(fgets(read_index,4,file_his),"\n");
    his_index = atoi(read_index);
    double prompt_flag=-1;


    while(1)
    {
      printf("\033[0;32m");
      if(prompt_flag == -1) printf("%s%s>",str,getCurrentDir());
      else
      {
        printf("%stook %fs>",str,prompt_flag);
        prompt_flag = -1;
      }
      printf("\033[0m");
      char* cmd=(char*)malloc(1000*sizeof(char));
      size_t x=1000;
      getline(&cmd,&x,stdin);
      cmd=strtok(cmd,"\n");
      if(cmd==NULL) continue;
      if(history_add(cmd) !=0)
      {
        strcpy(history[his_index],cmd);
        // printf("%s\n",history[his_index]);
        his_index = (his_index+1) % 20;
      }
      char* semicolon_token[10];
      char* token_semi = strtok(cmd,";");
      semicolon_token[0] = token_semi;
      if(semicolon_token[0] == NULL) continue;
      int count = 0;
      token_semi = strtok(NULL,";");
      while(token_semi != NULL)
      {
        count++;
        semicolon_token[count] = token_semi;
        // printf("%s\n",semicolon_token[cmd_num]);
        // cmd_num++;
        token_semi = strtok(NULL,";");
      }
      count++;
      // printf("%d\n",count);
      // for(int i=0;i<count;i++) printf("%s\n",semicolon_token[i]);
      for(int i=0;i<count;i++)
      {
        int num_and=0;
        for(int j=0;j<strlen(semicolon_token[i]);j++)
        {
          if(semicolon_token[i][j] == '&') num_and++;
        }
        // printf("%d\n",num_and);
        char* token_and = strtok(semicolon_token[i],"&");
        char* and_token[10];
        and_token[0] = token_and;
        if(and_token[0] == NULL) continue;
        int count_and = 0;
        token_and = strtok(NULL,"&");
        while(token_and != NULL)
        {
          count_and++;
          and_token[count_and] = token_and;
          // printf("%s\n",semicolon_token[cmd_num]);
          // cmd_num++;
          token_and = strtok(NULL,"&");
        }
        count_and++;
        // token_and = strtok(NULL,"&");
        for(int j=0;j<count_and;j++)
        {
          char *tokenize = strtok(and_token[j], " \t\r\n");
          // printf("%s\n",tokenize);
          char *commands[100];
          commands[0] = tokenize;
          if(commands[0] == NULL) continue;
          int cmd_num = 0;
          tokenize = strtok(NULL," \t\r\n");
          while(tokenize != NULL)
          {
            cmd_num++;
            commands[cmd_num] = tokenize;
            // printf("%s\n",commands[cmd_num]);
            // cmd_num++;
            tokenize = strtok(NULL," \t\r\n");
          }
          cmd_num++;
          // for(int i=0;i<cmd_num;i++) printf("%s\n",commands[i]);
          if(strcmp(commands[0],"pwd") == 0){
            pwd_();
          }
          else if(strcmp(commands[0],"echo") == 0){
            print_echo(commands,cmd_num);
          }
          else if(strcmp(commands[0],"cd") == 0){
            cd(commands,cmd_num);
          }
          else if(strcmp(commands[0],"history") == 0){
            history_();
          }
          else if(strcmp(commands[0],"exit")==0){
            history_save(0);
          }
          else if(strcmp(commands[0],"ls")==0){
            ls(commands,cmd_num);
          }
          else if(strcmp(commands[0],"pinfo")==0){
            pinfo(commands,cmd_num,shell_pid);
          }
          else if(strcmp(commands[0],"discover")==0){
            // printf("%s\n",commands[0]);
            discover(commands,cmd_num);
          }
          else
          {
            if(j < num_and)
            {
              process(commands,cmd_num,1);
              prompt_flag = -1;
            }
            else
            {
              time_t begin = time(NULL);
              process(commands,cmd_num,0);
              time_t end = time(NULL);
              prompt_flag=(end - begin);
              if(prompt_flag < 1) prompt_flag = -1;
            }
          }
        }
      }
    }
}