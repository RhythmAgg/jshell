
typedef struct node{
    char process_name[300];
    int pid_process;
    struct node* next;
    int process_num;
}bg_process;
typedef struct fg_running{
  char name[300];
  int pid_fg;
}fg_running;
extern bg_process* head;
extern bg_process* tail;
extern int bg_process_size;
extern fg_running* fg_node;
extern int last_node;


void process(char** commands,int cmd_num,int back);
