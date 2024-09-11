/*6. Write a program that creates a child process, and then in the child closes standard output
(STDOUTFILENO). What happens if the child calls printf() to print some output after
closing the descriptor?*/

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	int rc = fork();
	if (rc < 0) {
		// fork failed
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (rc == 0) {
		// child: redirect standard output 
		close(STDOUT_FILENO);
        printf("Hello World!\n");
	} else {
		// parent goes down this path (main)
		int rc_wait = wait(NULL);
	}
	return 0;
}

// printf will not output in this program