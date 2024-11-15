/* 2. Write another program using fork(). The child process should print “hello”; the parent
process should print “goodbye”. You should try to ensure that the child process always prints
first; can you do this without calling wait() in the parent? */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char* argv[]) { 
    printf("hello (pid:%d)\n", (int)getpid());

    int rc = fork();
    if (rc < 0) { fprintf(stderr, "Fork failed\n"); exit(1); }
    else if (rc == 0) { // child
        printf("hello\n");
    } else { // parent
        int rc_wait = wait(NULL);
        printf("goodbye\n");
    }
    return 0;
}

// for the child process to ALWAYS print first,
// wait() MUST be called by the parent process