/* 4. Now write a program that uses wait() to wait for the child process to finish in the parent.
 * What does wait() return? What happens if you use wait() in the child? */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char* argv[]) { 
    printf("hello (pid:%d)\n", (int)getpid());

    int rc = fork();
    if (rc < 0) { fprintf(stderr, "Fork failed\n"); exit(1); }
    else if (rc == 0) { // child {
        printf("child (pid:%d)\n", (int)getpid());
    } else {
        int rc_wait = wait(NULL);
        printf("parent of %d (rc_wait:%d) (pid:%d)\n\n", rc, rc_wait, (int)getpid());
    }
    return 0;
}

// wait returns the pid
// a wait() during child
// will cause a deadlock
