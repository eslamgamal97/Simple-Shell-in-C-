/* ************* Simple Shell on Linux Project *********************
    written by
        eslam gamal elsayed

    Email:
        eslamgamal.201441@gmail.com
*/

// including important library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //for chdir function
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

//declare variables
char command[1024];       // for user command
char change_dir[1024];   // for new directory by using cd
int i = 0;               // counter for cd command using / only
int j = 1;               // counter for cd command using ./
FILE* F;                 // pointer to log file

//Declaration of functions
void sigchld_handler(int sig);                              //executes when catching a signal from child process
void change_directory();                                     //function for changing current directory
void commandW_dash(char* dash_find);                         //function for commands with dash
void commandW_and(char* and_find,char* space_find);         //function for commands with and
void commandFunc(char* newline_find,char* space_find);      //function for command like pwd,ls,firefox,..etc


int main()
{
    //to start a blank file everytime the program is executed
    F = fopen("log_file.txt","w");  //open and discard the current contents
    fprintf(F,"");                  // write empty string in the log file
    fclose(F);                      // close the log file

    while(1)
    {
        printf("$>> "); //shell arrow for cursor
        fgets(command,1024,stdin); // to get the command from user
        //to exit shell
        if(command[0] == 'e' && command[1] == 'x' && command[2] == 'i' && command[3] == 't')
        {
            exit(1);
        }
        //to change directory
        else if(command[0] == 'c' && command[1] == 'd')
        {
            change_directory();
        }
        else
        {
            char* dash = strchr(command,'-');  //search on dash in the command like ls -l command
            //if dash found
            if(dash != NULL)
            {
                commandW_dash(dash);
            }
            else
            {
                char* space_find = strchr(command,' ');       //search on space in the command
                char* newline_find = strchr(command,'\n');    //search on newline in the command
                char* and_find = strchr(command,'&');         //search on and(&) in the command
                //if & is found
                if(and_find != NULL)
                {
                    commandW_and(and_find,space_find);
                }
                else
                {
                    commandFunc(newline_find,space_find);
                }
            }
        }

    }
    return 0;
}

void sigchld_handler(int sig)
{
    F = fopen("log_file.txt","a");
    fprintf(F,"Child process is terminated\n");
    fclose(F);
}

void change_directory()
{
    int pid_cd; //pid for cd command
    char* slash_find = strchr(command,'/');  //search on / in the path of new directory
    char* dot_find = strchr(command,'.');    //search on . in the path of new directory
    // if dot found
    if(dot_find != NULL)
    {
       j = 1;  // reinitialize the counter
       while(*(dot_find+j) != '\n')  //load the path of the new directory form command array to change_dir array
       {
            change_dir[i] = *(dot_find+j);
            i++;
            j++;
       }
       change_dir[i] = '\0';        //to end the string of new path
       pid_cd = fork();             //create a child process for cd command
       if(pid_cd == 0)
       {
            //here is the child
            chdir(change_dir);
       }
       else if(pid_cd > 0)
       {
            // here is the parent
            signal(SIGCHLD,sigchld_handler);   //to catch signal when  cd command is terminated
            waitpid(pid_cd,NULL,WUNTRACED);    // the parent must wait the child until is terminated
            exit(1);
       }
       else
       {
            //failed forking chiled
            printf("Fork failed\n");
       }

    }
    //if slash found but there is not a dot
    else if(slash_find != NULL)
    {
       i = 0; // reinitializing i if you use ./ after using /
       while(*(slash_find+i) != '\n')     //load the path of the new directory form command array to change_dir array
       {
            change_dir[i] = *(slash_find+i);
            i++;
       }
       change_dir[i] = '\0';             //to end the string of new path
       pid_cd = fork();                  //create a child process for cd command
       if(pid_cd == 0)
       {
            //here is a child
            chdir(change_dir);
       }
       else if(pid_cd > 0)
       {
            //here is a parent
            waitpid(pid_cd,NULL,WUNTRACED);     //when parent catch signal from child, sigchld_handler will be executed
            signal(SIGCHLD,sigchld_handler);    //parent waits child process until termination
            exit(1);
       }
       else
       {
            //Failed forking child
            printf("Fork failed\n");
       }
    }
}

void commandW_dash(char* dash_find)
{
    int pidW_dash;     // pid for command with dash
    char c[3],s[2];    //two arrays for first and second parts of command ex: ls -l so
    c[0] = *dash_find;      //copy dash from command
    c[1] = '\0';
    s[0] = *(dash_find+1);  //copy next character after dash
    s[1] = '\0';
    strcat(c,s);        //cat two characters
    *dash_find = '\0';  //to end ls command
    char* space_find = strchr(command,' ');  //search on space in command
    //if space found
    if(space_find != NULL)
    {
        *space_find = '\0';
    }
    pidW_dash = fork();   //create a child process
    if(pidW_dash == 0)
    {
        //here is a child
        char cmd[1024];
        strcpy(cmd,command);
        char* argv[3];
        argv[0] = command;
        argv[1] = c;
        argv[2] = NULL;
        execvp(cmd,argv);
    }
    else if(pidW_dash > 0)
    {
        //here is a parent
        signal(SIGCHLD,sigchld_handler);   //when parent catch signal from child, sigchld_handler will be executed
        waitpid(pidW_dash,NULL,WUNTRACED);       //parent waits child process until termination
    }
    else
    {
        //errer: faild forking child
        printf("Fork failed\n");
    }
}

void commandW_and(char* and_find,char* space_find)
{
    int pidW_and;     //pid for command with and
    //if space is found
    if(space_find != NULL)
    {
        *space_find = '\0';
    }
    else
    {
        *and_find = '\0';
    }
    pidW_and = fork();  //create a child process
    if(pidW_and == 0)
    {
        //here is a child
        char cmd[1024];
        strcpy(cmd,command);
        char* argv[3];
        argv[0] = command;
        argv[1] = NULL;
        argv[2] = NULL;
        execvp(cmd,argv);
    }
    else if(pidW_and > 0)
    {
        //here the parent
        signal(SIGCHLD,sigchld_handler);       //when parent catch signal from child, sigchld_handler will be executed
    }
    else
    {
        //Error: Failed forking a child
        printf("Fork failed\n");
    }
}

void commandFunc(char* newline_find,char* space_find)
{
    int pid;  //pid for command like pwd,ls,firefox,....etc
    //if space is found
    if(space_find != NULL)
    {
        *space_find = '\0';
    }
    //if newline is found
    if(newline_find != NULL)
    {
        *newline_find = '\0';
    }
    pid = fork();  //create a child process
    if(pid == 0)
    {
        //here is a child
        char cmd[1024];
        strcpy(cmd,command);
        char* argv[3];
        argv[0] = command;
        argv[1] = NULL;
        argv[2] = NULL;
        execvp(cmd,argv);
    }
    else if (pid > 0)
    {
        //here is a parent
        signal(SIGCHLD,sigchld_handler);  //when parent catch signal from child, sigchld_handler will be executed
        waitpid(pid,NULL,WUNTRACED);      //parent waits child process until termination
    }
    else
    {
        //Error: Failed forking a child
        printf("Fork failed\n");
    }
}
