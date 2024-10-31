/* 1. Write a program that calls fork(). Before calling fork(), have the main process
access a variable (e.g., x) and set its value to something (e.g., 100). What value is the
variable in the child process? What happens to the variable when both the child and parent
change x? */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, const char* argv[]) {
	printf("before fork (pid:%d)\n", (int)getpid());
	int x = 100;
	int rc = fork();
	if (rc < 0) { fprintf(stderr, "Fork failed\n"); exit(1); }
	else if (rc == 0) { // child
		printf("Child, x: %d\n", x); x = 200; printf("Child, now x: %d\n", x);
		printf("child (pid:%d)\n", (int)getpid());
	} else { // parent
		printf("parent, x: %d\n", x); x = 200; printf("Parent, now x: %d\n", x);
		printf("parent of %d (pid:%d)\n\n", rc, (int)getpid());
	}
	return 0;
}
