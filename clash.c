#include <stdlib.h> //exit(3)
#include <stdio.h>
#include <unistd.h> //fork(2), execlp(3), execvp(3)
#include <sys/types.h> //fork(2), wait(2)
#include <sys/wait.h> //wait(2), waitpid(2)
#include <string.h>

#include "plist.h"

#define CWD_LEN 256

//functions definitions

void input_loop();
void run_command(char*** input_arguments, int* input_arguments_count, char* input_str);
static void fatal_exit(const char* message);

bool wait_single_background_process(pid_t pid, const char* cmd);

//end of functions definitions

int main()
{
    input_loop();
    return 0;
}

void input_loop()
{
    char cwd_str[CWD_LEN]; // array for current directory
    char *token;
    char** input_arguments; // single words from input
    int input_arguments_count;

    char input_str[256];
    for(int i = 0; i < sizeof(input_str); i++) // clean input string
    {
        input_str[i] = '\0';
    }
    int input_str_count;

    while(!feof(stdin)) // loop runs until Ctrl-D is pressed
    {
        getcwd(cwd_str, CWD_LEN);
        // TODO add NULL check

        printf("%s: ", cwd_str);

        int i = getchar();
        input_str_count = 0;

        while(i != '\n' && i != EOF) // loop runs until the enter or ctrl-d is pressed
        {
            if(i != EOF)
            {
                //read_input(&input_str);
                input_str[input_str_count] = (char) i;
                input_str_count ++;
            }
            i = getchar();
        }

        char* input_str_copy = malloc(strlen(input_str) * sizeof(char));
        strcpy(input_str_copy, input_str);
        token = strtok(input_str, " ");
        input_arguments_count = 0; // set count to 0 every loop
        input_arguments = NULL; // instanciate argument list

        while(token != NULL)
        {
            input_arguments_count ++;
            input_arguments = (char**) realloc(input_arguments, sizeof(char*) * input_arguments_count); // broaden argument list for append
            // TODO check

            input_arguments[input_arguments_count - 1] = token; // append to argument list

            //printf("~ %s    ", token);
            token = strtok(NULL, " ");
        }

        // printf("\n");

        // for(int i = 0; i < input_arguments_count; i++)
        // {
        //     printf("~ %s    ", input_arguments[i]);
        // }

        // printf("\n");

        run_command(&input_arguments, &input_arguments_count, input_str_copy);

        free(input_arguments);
        for(int i = 0; i < sizeof(input_str); i++) // clean input string
        {
            input_str[i] = '\0';
        }
    }
}

static void fatal_exit(const char* message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void run_command(char*** input_arguments, int* input_arguments_count, char* input_str)
{
    // TODO check if the count is too small - 0
    if(*input_arguments_count == 0)
    {
        return;
    }

    //case that the command is valid
    pid_t pid;
    pid_t child_pid;

    pid = fork();
    if(pid < 0) fatal_exit("fork");

    if (pid == 0) // child process
    {
        char* args[*input_arguments_count + 1]; // creating an array of arguments for execvp
        for (int i = 0; i < *input_arguments_count; i ++)
        {
            args[i] = (*input_arguments)[i];
        }
        args[*input_arguments_count] = NULL; // last elemnt in the array must be null

        printf("%s\n", (*input_arguments)[(*input_arguments_count) - 1]);

        bool is_bg = (strcmp((*input_arguments)[(*input_arguments_count) - 1], "&") == 0);
        printf("%d\n", is_bg);

        if (is_bg)
        {
            printf("hintergrund");
            //input_str[strlen(input_str) - 1] = '\0';
            //insertElement(pid, input_str);
            //child_pid = pid;
        }

        execvp(args[0], args);
        fatal_exit("execvp");
    }
    else // parent process
    {
        int status;
        int res;

        // if (strcmp((*input_arguments)[(*input_arguments_count) - 1], "&") == 0)
        // {
        //     res = waitpid(-1, &status, WNOHANG);
        //     char* buf = malloc(256 * sizeof(char));
        //     removeElement(child_pid, buf, 256);
        //     if (res < 0) fatal_exit("wait");
        //     printf("Exitstatus [%s]: %d\n", input_str, status);
        //     return;
        // }

        walkList(&wait_single_background_process);

        res = wait(&status);
        if (res < 0) fatal_exit("wait");
        printf("Exitstatus [%s]: %d\n", input_str, status);
    }
}

bool wait_single_background_process(pid_t pid, const char* cmd)
{
    int status;
    int res = waitpid(pid, &status, WNOHANG);

    if (res < 0)
    {
         fatal_exit("wait");
    }

    bool running = !WIFEXITED(status);
    if (!running) printf("Exitstatus [%s]: %d\n", cmd, status);
    return running;
}