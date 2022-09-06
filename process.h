
typedef struct node{
    char process_name[300];
    int pid_process;
    struct node* next;
}bg_process;
bg_process* head;
bg_process* tail;
int bg_process_size;

void process(char** commands,int cmd_num,int back);
