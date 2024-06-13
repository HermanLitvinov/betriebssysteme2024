#include <stdlib.h> //exit(3)
#include <stdio.h>
#include <unistd.h> //fork(2), execlp(3), execvp(3)
#include <sys/types.h> //fork(2), wait(2)
#include <sys/wait.h> //wait(2), waitpid(2)
#include <string.h>
#include <errno.h>

#include "plist.h"

#define BUFFER_SIZE 1337

static void input_loop();
static void fatal_exit(const char* message);
static void collect_terminated_background_processes();
static void new_input_line();
static bool print_background_job(pid_t pid, const char *cmdLine);


int main(void) { // No arguments accepted
	input_loop();
	exit(EXIT_SUCCESS);
}

static void input_loop() {
		while (1) {		
		collect_terminated_background_processes();
		new_input_line();
		
		char buffer[BUFFER_SIZE + 1]; // buffer for stdin including '\0'
	
		// Read a line of input from the user
		if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
			if (feof(stdin)) break;
			fatal_exit("fgets");
		}
		
		// Check user input length
		// We took the following logic from the discussed wsort solution in class (T08)
		if (strlen(buffer) == BUFFER_SIZE && buffer[BUFFER_SIZE-1] != '\n') {
			fprintf(stderr, "Input line too long\n"); // Not fatal, user gets another prompt
			
            int c;
            do {
                c = fgetc(stdin);
            } while (c != EOF && c != '\n');
            
            if (ferror(stdin)) fatal_exit("fgetc");
            
            continue; // Ignore this line
		}
		
		// If no command was entered, continue to next iteration
		if (strlen(buffer) < 2) {
			continue;
		}
	
		// Remove newline character if present
		buffer[strcspn(buffer, "\n")] = 0;
		
		// & indicates a background process
		bool run_in_background = false;
		if (buffer[strlen(buffer)-1] == '&') {
			buffer[strlen(buffer)-1] = '\0';
			run_in_background = true;
		}
		
		/* Problem: We need to tokenize the input so the puffer is divided by strtok()
					BUT we still need the command line! For example to print the exitstatus or to save it in the list (if it's a background process)
		   Solution: Use a copy of the buffer here
		*/
		
		char buffer_copy[sizeof(buffer)];
		strcpy(buffer_copy, buffer);
		
		// Tokenize the input line
		char *argv[(BUFFER_SIZE / 2) + 2]; // Assume max arguments is half of buffer size, and +2 because of '\0' and necessary NULL
		int i = 0;
		argv[i++] = strtok(buffer, " \t");
		while ((argv[i++] = strtok(NULL, " \t")) != NULL) {
			continue;
		}
		
		// If no command was entered, continue to next iteration
		if (argv[0] == NULL) {
			continue;
		}
	
		// Handle the "cd" built-in command
		if (strcmp(argv[0], "cd") == 0) {
			if (argv[1] == NULL || argv[2] != NULL) { // not specified if necessary
				fprintf(stderr, "cd: missing argument\n");
			} else {
				if (chdir(argv[1]) != 0) {
					perror("chdir");
				}
			}
			continue;
		}
		
		// "jobs" command
		if (strcmp(argv[0], "jobs") == 0) {
			walkList(print_background_job);
			continue;
		}
		
		// Fork a new child process
		pid_t pid = fork();
		if (pid < 0) {
			fatal_exit("fork");
		} else if (pid == 0) {
			execvp(argv[0], argv);
			fatal_exit("exec");
		}
		
		if (run_in_background) {
			// Background process
			// Insert into list with error handling -> plist.c to how insertElement() returns
			if (-2 == insertElement(pid, buffer_copy)) {
				fprintf(stderr, "list is full");
				exit(EXIT_FAILURE);
			}
		} else {
			// Parent process, wait for pid 
			int status;
			if (waitpid(pid, &status, 0) < 0) {
				fatal_exit("waitpid");
			}
			// Print Exitstatus if there is one
			printf("Exitstatus [%s] = %d\n", buffer_copy, WIFEXITED(status) ? WEXITSTATUS(status) : printf("No exitstatus [%s]\n", buffer_copy));
		}
	}
}

static void fatal_exit(const char* message) {
	perror(message);
	exit(EXIT_FAILURE);
}

static void collect_terminated_background_processes() {
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) != 0) {
		if (pid < 0) {
			if (errno == ECHILD) {
				break; // No child processes
			}
			fatal_exit("waitpid");
		}
		
		char cmdLine[BUFFER_SIZE + 1];
		
		if (removeElement(pid, cmdLine, sizeof(cmdLine)) < 0) {
			continue;
		}
		
		// Print Exitstatus if there is one
		printf("Exitstatus [%s] = %d\n", cmdLine, WIFEXITED(status) ? WEXITSTATUS(status) : printf("No exitstatus [%s]\n", cmdLine));
    }
}

/*  Give the user a new input possibility, e.g. /proj/bs/: echo test
	Use getcwd as described here: https://man7.org/linux/man-pages/man3/getcwd.3.html (*,under Description)
	Not using getcwd(NULL, 0); as it is not part of the POSIX specification!
*/
static void new_input_line() {
	size_t path_max = 4096; // most common, see reference *
	char *path = NULL;
	
	while (1) {
		path = realloc(path, path_max);
		if (path == NULL) {
			fatal_exit("realloc");
		}
		
		// Get the current working directory
		if (getcwd(path, path_max) != NULL) {
			break; // Sucess
		}
		
		if (errno != ERANGE) {
			fatal_exit("getcwd");
		}
		path_max *= 2; // No success, try again with more buffer memory
	}
	
	printf("%s: ", path);
	fflush(stdout);
	
	free(path);
}

static bool print_background_job(pid_t pid, const char *cmdLine) {
	printf("PID: %d, Command: %s\n", (int)pid, cmdLine);
	return false;
}

