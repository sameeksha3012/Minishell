#include "header.h"
typedef struct node
{
    pid_t pid;
    char str[25];
    struct node *link;
} Slist;
int EX_flag = 0;
char new_prompt[25];
   extern char input_str[25];
Slist *head = NULL;
pid_t pid;
// pid_t child_pid;
// char sll_com[25]="pranav";
void my_handler(int signum)
{
    if (signum == SIGINT)
    {
        if (EX_flag == 0)
        {
            printf("\n");
            printf("%s", new_prompt);
            fflush(stdout);
        }
        else
            EX_flag = 0;
    }
    else if (signum == SIGTSTP)
    {
        if (EX_flag == 0)
        {
            printf("\n");
            printf("%s", new_prompt);
            fflush(stdout);
        }
        else
        {
          // printf("Pid is %d\n",child_pid);
            // printf("Pid is %s\n",sll_com);
            EX_flag = 0;
            insert_at_first(pid,input_str);
        }
    }
    else if (signum == SIGCHLD)
    {
        int status;
        waitpid(-1, &status, WNOHANG);
        delete_first();
    }
}
void scan_input(char *prompt, char *input_string)
{

    char command[25];
    char *external_comm[152];

    signal(SIGINT, my_handler);
    signal(SIGTSTP, my_handler);
    extract_external_commands(external_comm);
    while (1)
    {
        strcpy(new_prompt, prompt);
        printf("%s", prompt);
        scanf(" %[^\n]", input_string);
        if (!strncmp(input_string, "ps1", 3))
        {
            // if (strchr(input_string, ' '))
            // {
            //     printf("Invalid command\n");
            // }
            // else
            //     strcpy(prompt, input_string + 4);
        }
        else
        {
            get_command(input_string, command);
            int res = check_command_type(command, external_comm);
            if (res == BUILTIN)
            {
                if (!strcmp(command, "fg"))
                {
                   int res= kill(head->pid, SIGCONT);
                   printf("The head of pid => %d\n",head->pid);
                   waitpid(head->pid, NULL, WUNTRACED);
                   printf("res is %d\n",res);
                    

                }
                else if (!strcmp(command, "bg"))
                {
                    signal(SIGCHLD, my_handler);
                    kill(head->pid, SIGCONT);
                }
                else if (!strcmp(command, "jobs"))
                {
                     print_list();
                }
                else
                    execute_internal_commands(input_string);
            }

            else if (res == EXTERNAL)
            {
                // printf("External command\n");
                 pid = fork();

                if (pid > 0)
                {
                    int status;
                    EX_flag = 1;
                    printf("From parent %d\n", pid);
                    waitpid(pid,&status, WUNTRACED);
                    /*if(WIFSTOPPED(status))
                    {   
                       // printf("kk\n");
                        insert_at_first(pid,input_string);
                    }*/
                    // wait(NULL);
                    //printf("Terminated or stopped\n");
                    pid = 0;
                }
                else if (pid == 0)
                {
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    execute_external_commands(input_string);
                    printf("Executed external cmd, %d\n", getpid());
                    exit(0);
                }
            }
        }
    }
}
int insert_at_first(pid_t pid,char *str)
{

    Slist *new = malloc(sizeof(Slist));
    if (new == NULL)
        return 1;
    new->pid = pid;
   // printf("The string is in insert %s    %d\n",sll_com,child_pid);
    strcpy(new->str, str);
    new->link = NULL;

    if (head == NULL)
    {
        head = new;
    }else{
        new->link=head;
        head=new;
    }
}
void print_list()
{
   // printf("LLL\n");
    if (head == NULL)
    {
      // printf("11111\n");
    }
    Slist *temp = head;

    while (temp)
    {
        printf("%s \n", temp->str);
        temp = temp->link;
    }

   // printf("NULL\n");
}
int delete_first()
{
    if (head == NULL)
        return 1;
    Slist *temp = head->link;
    free(head);
    head = temp;
    return 0;
}
