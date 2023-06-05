#include "headers.h"

char home_dir[300];
char prev_dir[300];
char curr_dir[300];
char *history[20];
int stdin_copy;
int stdout_copy;
int stdappend_copy;
int his_index = 0;
pid_t shell_pid;
bg_process *head;
bg_process *tail;
int bg_process_size;
int control_c = 0;
fg_running* fg_node;
int last_node=0;
char str[1000];
double prompt_flag = 0;


char* prompt();

void print(){
  bg_process* ptr = head->next;
  while(ptr != NULL)
  {
    printf("[%d] %s\n",ptr->process_num,ptr->process_name);
    ptr = ptr->next;
  }
}
//****************************************************************
void die(const char *s) {
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char* input_cmd()
{
  char *inp = malloc(sizeof(char) * 1024);
    char c;
    while (1) {
        setbuf(stdout, NULL);
        enableRawMode();
        printf("\033[0;32m");
        printf("%s>",prompt());
        printf("\033[0m");
        prompt_flag = 0;

        memset(inp, '\0', 1024);
        int pt = 0;
        while (read(STDIN_FILENO, &c, 1) == 1) {
            if (iscntrl(c)) {
                if (c == 10) break;
                else if (c == 127) { // backspace
                    if (pt > 0) {
                        if (inp[pt-1] == 9) {
                            for (int i = 0; i < 7; i++) {
                                printf("\b");
                            }
                        }
                        inp[--pt] = '\0';
                        printf("\b \b");
                    }
                } else if (c == 9) { // TAB character
                    char tmp[1024];
                    strcpy(tmp,inp);
                    strcpy(inp,tab(tmp));
                    pt = strlen(inp);
                    printf("\033[0;32m");
                    printf("\r%s>",prompt());
                    printf("\033[0m");
                    printf("%s",inp);
                } else if (c == 4) { // ctrl D
                    exit(0);
                } else {
                    printf("%d\n", c);
                }
            } else {
                inp[pt++] = c;
                printf("%c", c);
            }
        }
        disableRawMode();
        return inp;
      }
}
//**********************************************************************

int min_(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

const char *getCurrentDir()
{
  char *temp;
  char *output;
  temp = (char *)malloc(sizeof(char) * 300);
  output = (char *)malloc(sizeof(char) * 300);
  if (strlen(curr_dir) < strlen(home_dir))
  {
    strcpy(output, curr_dir);
    return output;
  }
  if (strncmp(curr_dir, home_dir, strlen(home_dir)) == 0)
  {
    strcat(output, "~");
  }
  strcat(output, &curr_dir[strlen(home_dir)]);
  return output;
}

char* prompt()
{
  char* tmp = (char*)malloc(sizeof(char)*300);
  strcpy(tmp,str);
  if (prompt_flag == 0)
    strcat(tmp,getCurrentDir());
  else
  {
    char time[20];
    // snprintf(time,20,"%f",prompt_flag);
    strcat(tmp,"took ");  
  }
  return tmp;
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
  // printf("\n");
  if(control_c == 1) return;
  int status;
  bg_process *ptr;
  bg_process *prev;
  prev = head;
  ptr = head->next;
  int exit_flag = 0;
  while (ptr != NULL)
  {
    if (waitpid(ptr->pid_process, &status, WNOHANG) != 0)
    {
      if (WIFEXITED(status))
      {
        exit_flag = 1;
      }
      printf("\n%s process with pid %d has exited %s\n", ptr->process_name, ptr->pid_process, (exit_flag) ? "normally" : "abnormally");
      prev->next = ptr->next;
      ptr = ptr->next;
      if (ptr == NULL)
        tail = prev;
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
  strcpy(history_sahi_jagah, home_dir);
  strcat(strcat(history_sahi_jagah, "/"), "history_logs.txt");
  int fd = open(history_sahi_jagah, O_WRONLY | O_CREAT | O_TRUNC, 0700);
  for (int i = 0; i < 20; i++)
  {
    // printf("%s\n",history[i]);
    write(fd, history[i], strlen(history[i]));
    write(fd, "\n", 1);
  }
  char write_index[4];
  sprintf(write_index, "%d", his_index);
  write(fd, write_index, strlen(write_index));
  // exit(sig);
}
void ctrl_c_handler(int sig)
{
  // control_c=1;
  return;
}
void ctrl_z_handler(int sig)
{
  return;
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
  signal(SIGCHLD, bg_handler);
  signal(SIGINT, ctrl_c_handler);
  signal(SIGTSTP,ctrl_z_handler);
  bg_process_size = 0;
  head = (bg_process *)malloc(sizeof(bg_process));
  tail = (bg_process *)malloc(sizeof(bg_process));
  head->next = NULL;
  head->pid_process = -1;
  tail = head;
  shell_pid = getpid();
  fg_node = (fg_running*)malloc(sizeof(fg_running));
  strcpy(fg_node->name,"./shell");
  fg_node->pid_fg = shell_pid;
  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  char username[300];
  strcpy(username, getUserName());
  strcpy(str,"<");
  getcwd(home_dir, 300);
  strcpy(prev_dir, home_dir);
  strcpy(curr_dir, home_dir);
  strcat(strcat(strcat(strcat(str, username), "@"), hostname), ":");
  FILE *file_his = fopen("history_logs.txt", "r");
  size_t len = 1000;
  for (int i = 0; i < 20; i++)
  {
    history[i] = (char *)malloc(sizeof(char) * 1000);
    fgets(history[i], 1000, file_his);
    history[i][strlen(history[i]) - 1] = '\0';
    // printf("ok\n");
    // printf("%s\n",history[i]);
  }
  // for(int i=0;i<20;i++) printf("%s\n",history[i]);

  char *read_index = (char *)malloc(sizeof(char) * 4);
  read_index = strtok(fgets(read_index, 4, file_his), "\n");
  his_index = atoi(read_index);

  while (1)
  {
    control_c = 0;
    // printf("\033[0;32m");
    // if (prompt_flag == 0)
    //   printf("%s%s>", str, getCurrentDir());
    // else
    // {
    //   printf("%stook %fs>", str, prompt_flag);
    //   prompt_flag = 0;
    // }
    // printf("%s>",prompt(str,prompt_flag));
    // prompt_flag = 0;
    // printf("\033[0m");
    char *cmd = (char *)malloc(1024 * sizeof(char));
    size_t x = 1024;
    // if (getline(&cmd, &x, stdin) == -1)
    // {
    //   history_save(0);
    //   exit(1);
    // }
    strcpy(cmd,input_cmd());
    printf("\n");
    cmd = strtok(cmd, "\n");
    if (cmd == NULL)
      continue;
    if (history_add(cmd) != 0)
    {
      strcpy(history[his_index], cmd);
      // printf("%s\n",history[his_index]);
      his_index = (his_index + 1) % 20;
    }
    char *semicolon_token[10];
    char *token_semi = strtok(cmd, ";");
    semicolon_token[0] = token_semi;
    if (semicolon_token[0] == NULL)
      continue;
    int count = 0;
    token_semi = strtok(NULL, ";");
    while (token_semi != NULL)
    {
      count++;
      semicolon_token[count] = token_semi;
      // printf("%s\n",semicolon_token[cmd_num]);
      // cmd_num++;
      token_semi = strtok(NULL, ";");
    }
    count++;
    // printf("%d\n",count);
    // for(int i=0;i<count;i++) printf("%s\n",semicolon_token[i]);
    for (int i = 0; i < count; i++)
    {
      int num_and = 0;
      for (int j = 0; j < strlen(semicolon_token[i]); j++)
      {
        if (semicolon_token[i][j] == '&')
          num_and++;
      }
      // printf("%d\n",num_and);
      char *token_and = strtok(semicolon_token[i], "&");
      char *and_token[10];
      and_token[0] = token_and;
      if (and_token[0] == NULL)
        continue;
      int count_and = 0;
      token_and = strtok(NULL, "&");
      while (token_and != NULL)
      {
        count_and++;
        and_token[count_and] = token_and;
        // printf("%s\n",semicolon_token[cmd_num]);
        // cmd_num++;
        token_and = strtok(NULL, "&");
      }
      count_and++;
      // token_and = strtok(NULL,"&");
      for (int j = 0; j < count_and; j++)
      {
        char *tokenize = strtok(and_token[j], " \t\r\n");
        // printf("%s\n",tokenize);
        int in_flag = -1, out_flag = -1, append_flag = -1;
        char *commands_[100];
        commands_[0] = tokenize;
        int pipe_num = 0;
        if (commands_[0] == NULL)
          continue;
        int cmd_num = 0;
        tokenize = strtok(NULL, " \t\r\n");
        while (tokenize != NULL)
        {
          cmd_num++;
          commands_[cmd_num] = tokenize;
          // if(strcmp(commands_[cmd_num],">")==0) out_flag = cmd_num;
          // if(strcmp(commands_[cmd_num],"<")==0) in_flag = cmd_num;
          // if(strcmp(commands_[cmd_num],">>")==0) append_flag = cmd_num;
          if (strcmp(commands_[cmd_num], "|") == 0)
            pipe_num++;
          // printf("%s\n",commands_[cmd_num]);
          // cmd_num++;
          tokenize = strtok(NULL, " \t\r\n");
        }
        cmd_num++;
        int file_in, file_out, file_append,pipe_input=0;
        char *commands[pipe_num + 1][100];
        int seg_index = 0;
        int end_a = cmd_num;
        int ind_commands = 0;
        for (int a = 0; a < end_a; a++)
        {
          if (strcmp(commands_[a], ">") == 0)
          {
            stdout_copy = dup(STDOUT_FILENO);
            file_out = open(commands_[a+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (file_out < 0)
            {
              perror("Error: ");
              continue;
            }
            out_flag = 1;
            dup2(file_out, STDOUT_FILENO);
            a++;
          }
          else if (strcmp(commands_[a], "<") == 0)
          {
            if(pipe_input){
              dup2(stdin_copy,STDIN_FILENO);
              pipe_input=0;
            }
            stdin_copy = dup(STDIN_FILENO);
            file_in = open(commands_[a + 1], O_RDONLY);
            if (file_in < 0)
            {
              perror("Error: ");
              continue;
            }
            in_flag = 1;
            dup2(file_in, STDIN_FILENO);
            a++;
          }
          else if (strcmp(commands_[a], ">>") == 0)
          {
            stdappend_copy = dup(STDOUT_FILENO);
            file_append = open(commands_[a + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (file_append < 0)
            {
              perror("Error: ");
              continue;
            }
            append_flag = 1;
            dup2(file_append, STDOUT_FILENO);
            a++;
          }
          else if (strcmp(commands_[a], "|") == 0)
          {
            int fild[2];
            if (pipe(fild) < 0)
            {
              perror("Error: ");
              continue;
            }
            // if (out_flag > 0)
            // {
            //   out_flag = 0;
            //   dup2(stdout_copy, STDOUT_FILENO);
            //   close(file_out);
            // }
            // if (append_flag > 0)
            // {
            //   append_flag = 0;
            //   dup2(stdappend_copy, STDOUT_FILENO);
            //   close(file_append);
            // }
            // stdout_copy = dup(STDOUT_FILENO);
            // dup2(fild[1], STDOUT_FILENO);
            if(out_flag == -1 && append_flag == -1){
              stdout_copy = dup(STDOUT_FILENO);
              stdappend_copy = dup(STDOUT_FILENO);
              dup2(fild[1], STDOUT_FILENO);
            }
            cmd_num = ind_commands;
            ind_commands = 0;
            if (strcmp(commands[seg_index][0], "pwd") == 0)
            {
              pwd_();
              printf("\n");
            }
            else if (strcmp(commands[seg_index][0], "echo") == 0)
            {
              print_echo(commands[seg_index], cmd_num);
              printf("\n");
            }
            else if (strcmp(commands[seg_index][0], "cd") == 0)
            {
              cd(commands[seg_index], cmd_num);
            }
            else if (strcmp(commands[seg_index][0], "history") == 0)
            {
              history_();
            }
            else if (strcmp(commands[seg_index][0], "exit") == 0)
            {
              history_save(0);
              exit(0);
            }
            else if (strcmp(commands[seg_index][0], "ls") == 0)
            {
              ls(commands[seg_index], cmd_num);
            }
            else if (strcmp(commands[seg_index][0], "pinfo") == 0)
            {
              pinfo(commands[seg_index], cmd_num, shell_pid);
            }
            else if (strcmp(commands[seg_index][0], "discover") == 0)
            {
              // printf("%s\n",commands[seg_index][0]);
              discover(commands[seg_index], cmd_num);
            }
            // else if(strcmp(commands[seg_index][0],"cat")==0){
            //   commands[seg_index][cmd_num++] = NULL;
            //   if(execvp(commands[seg_index][0],commands[seg_index])==-1){
            //     perror("Error: ");
            //   }
            // }
            else if (strcmp(commands[seg_index][0], "jobs") == 0)
            {
              job(commands[seg_index], cmd_num);
            }
            else if (strcmp(commands[seg_index][0], "sig") == 0)
            {
              sig(commands[seg_index], cmd_num);
            }
            else if (strcmp(commands[seg_index][0], "fg") == 0)
            {
              fg_(commands[seg_index], cmd_num);
            }
            else if (strcmp(commands[seg_index][0], "bg") == 0)
            {
              bg_(commands[seg_index], cmd_num);
            }
            else if(strcmp(commands[seg_index][0], "print") == 0)
            {
              print();
            }
            else
            {
              if (j < num_and)
              {
                process(commands[seg_index], cmd_num, 1);
                prompt_flag = 0;
              }
              else
              {
                time_t begin = time(NULL);
                process(commands[seg_index], cmd_num, 0);
                time_t end = time(NULL);
                prompt_flag += (end - begin);
                if (prompt_flag < 1)
                  prompt_flag = 0;
              }
            }
            close(fild[1]);
            dup2(stdout_copy,STDOUT_FILENO);
            if (in_flag > 0)
            {
              in_flag = 0;
              dup2(stdin_copy, STDIN_FILENO);
              close(file_in);
            }
            if(pipe_input){
              dup2(stdin_copy, STDIN_FILENO);
            }
            stdin_copy = dup(STDIN_FILENO);
            dup2(fild[0], STDIN_FILENO);
            pipe_input = 1;
            out_flag=-1;in_flag=-1;append_flag=-1;
            seg_index++;
          }
          else
          {
            commands[seg_index][ind_commands] = (char *)malloc(sizeof(char) * 300);
            strcpy(commands[seg_index][ind_commands++], commands_[a]);
            // printf("%s\n",commands[seg_index][ind_commands-1]);
          }
        }
        
        if(ind_commands == 0) continue;
        cmd_num = ind_commands;
        if (strcmp(commands[seg_index][0], "pwd") == 0)
        {
          pwd_();
        }
        else if (strcmp(commands[seg_index][0], "echo") == 0)
        {
          print_echo(commands[seg_index], cmd_num);
        }
        else if (strcmp(commands[seg_index][0], "cd") == 0)
        {
          cd(commands[seg_index], cmd_num);
        }
        else if (strcmp(commands[seg_index][0], "history") == 0)
        {
          history_();
        }
        else if (strcmp(commands[seg_index][0], "exit") == 0)
        {
          history_save(0);
          exit(0);
        }
        else if (strcmp(commands[seg_index][0], "ls") == 0)
        {
          ls(commands[seg_index], cmd_num);
        }
        else if (strcmp(commands[seg_index][0], "pinfo") == 0)
        {
          pinfo(commands[seg_index], cmd_num, shell_pid);
        }
        else if (strcmp(commands[seg_index][0], "discover") == 0)
        {
          // printf("%s\n",commands[seg_index][0]);
          discover(commands[seg_index], cmd_num);
        }
        // else if(strcmp(commands[seg_index][0],"cat")==0){
        //   commands[seg_index][cmd_num++] = NULL;
        //   if(execvp(commands[seg_index][0],commands[seg_index])==-1){
        //     perror("Error: ");
        //   }
        // }
        else if (strcmp(commands[seg_index][0], "jobs") == 0)
        {
          job(commands[seg_index], cmd_num);
        }
        else if (strcmp(commands[seg_index][0], "sig") == 0)
        {
          sig(commands[seg_index], cmd_num);
        }
        else if (strcmp(commands[seg_index][0], "fg") == 0)
        {
          fg_(commands[seg_index], cmd_num);
        }
        else if (strcmp(commands[seg_index][0], "bg") == 0)
        {
          bg_(commands[seg_index], cmd_num);
        }
        else if(strcmp(commands[seg_index][0], "print") == 0)
        {
          print();
        }
        else
        {
          if (j < num_and)
          {
            process(commands[seg_index], cmd_num, 1);
            prompt_flag = 0;
          }
          else
          {
            time_t begin = time(NULL);
            process(commands[seg_index], cmd_num, 0);
            time_t end = time(NULL);
            prompt_flag += (end - begin);
            if (prompt_flag < 1)
              prompt_flag = 0;
          }
        }
        if (in_flag > 0)
        {
          dup2(stdin_copy, STDIN_FILENO);
          close(file_in);
        }
        if (out_flag > 0)
        {
          dup2(stdout_copy, STDOUT_FILENO);
          close(file_out);
        }
        if (append_flag > 0)
        {
          dup2(stdappend_copy, STDOUT_FILENO);
          close(file_append);
        }
        if(pipe_input>0){
          dup2(stdin_copy,STDIN_FILENO);
        }
      }
    }
  }
}