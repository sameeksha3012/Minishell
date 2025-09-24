#include "header.h"
void extract_external_commands(char *external_commands[152])
{
    int fd = open("extcommand.txt", O_RDONLY);
    char buf;
    int i = 0, j = 0;
    char comm[25];
    while (read(fd, &buf, 1) > 0)
    {
        if (buf == -1)
            break;
        if (buf == '\n')
        {
            comm[i] = '\0';
            external_commands[j] = malloc(i);
            strcpy(external_commands[j], comm);
            i = 0;
            j++;
        }
        else
            comm[i++] = buf;
    }
}

void get_command(char *input_string, char *command)
{
    int i = 0;
    while (input_string[i] != ' ' && input_string[i] != '\0')
    {
        command[i] = input_string[i];
        i++;
    }
    command[i] = '\0';
}

int check_command_type(char *command,char **external_comm)
{   
    
    char *internal_comm[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                             "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                             "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","fg","bg","jobs", NULL};
    
    int i=0;
    while(internal_comm[i])
    {
        if(!strcmp(command,internal_comm[i]))
        return BUILTIN;
        i++;
    }
   
    for(i=0;i<152;i++)
    {
        if(!strcmp(command,external_comm[i]))
        return  EXTERNAL;
    }

    return NO_COMMAND;
    
    
}

void execute_internal_commands(char *input_string)
{
    if(!strncmp(input_string,"exit",4))
    exit(0);
    else if(!strncmp(input_string,"pwd",3))
    {
        char str[50];
        getcwd(str,50);
        printf("%s\n",str);
    }
    else if(!strncmp(input_string,"cd",2))
    {
        chdir(input_string+3);
    }
    else if(!strncmp(input_string,"echo",4))
    {
        if(strstr(input_string,"$$")!=NULL)
        {
          printf("%d\n",getpid());
        }
        else if(strstr(input_string,"$?")!=NULL)
        {
            char *buf;
            buf=getenv("SHELL");
            printf("%s\n",buf);
        }
    }
}
void execute_external_commands(char *input_string)
{  
    //printf("inside external command executaion\n");
    int pipeflag=0;
    char *input[25];
    int i = 0, j = 0;
    char comm[25];
    //printf("input string is %s\n",input_string);
   
    for(int k=0;k<=strlen(input_string);k++)
    {   
       // printf("%d\n",i);
        if(input_string[k]=='|')
        {   
             pipeflag=1;
        }
      
        if (input_string[k] == ' ' || input_string[k] == '\0')
        {
            comm[i] = '\0';
            input[j] = malloc(i);
            strcpy(input[j], comm);
            //printf("%s and j is %d\n",input[j],j);
            i = 0;
            j++;
        }
        else
            comm[i++] = input_string[k];
    }
    input[j]=NULL;
    //printf("J before exe is %d\n",j);
   
    if(pipeflag)
    {   
       execute_pipe_command(j,input);
    //    exit(0);
    }
    else
    {
        execute_single_external_command(j,input);
        // exit(0);
    }

    printf("Done\n");
}

int execute_pipe_command(int size,char *command[])
{     
    //printf("size is %d\n",size);
    // for(int i=0;i<size;i++)
    // {
    //     printf("%s\n",command[i]);
    // }
   int index[size];
    int cmd_count = 1;
    index[0] = 0;

   
    for (int i = 0; i < size; i++)
    {
        if (strcmp(command[i], "|") == 0)
        {
            command[i] = NULL;
            index[cmd_count++] = i + 1;
        }
    }
    //printf("cmd count is %d\n",cmd_count);
    int pipes[cmd_count - 1][2]; 

    // Create all pipes
    for (int i = 0; i < cmd_count - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(1);
        }
    }

    for (int i = 0; i < cmd_count; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
           

            // If not the first command, get input from previous pipe
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // If not the last command, output to next pipe
            if (i < cmd_count - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // pipes for child
            for (int j = 0; j < cmd_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
          
            execvp(command[index[i]], &command[index[i]]);
            perror("execvp failed");
            exit(1);
        }
        else if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
    }

    // pipes for parent
    for (int i = 0; i < cmd_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < cmd_count; i++)
    {
        wait(NULL);
    }
    exit(0);
    //return 1;
}

int execute_single_external_command(int size,char *command[])
{  
    pid_t ret ;
    pid_t pid = fork() ;
    if(pid >0)
    {
        ret = wait(NULL) ;
        //printf("Child terminated\n");
    }
    else if(pid == 0)
    {
        execvp(command[0],command) ;
    }
    exit(0);

}



