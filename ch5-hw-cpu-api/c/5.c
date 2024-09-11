/*5. Write a slight modification of the previous program, 
 *this time using waitpid()wait(). When would waitpid() be useful?*/

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
        int info;
        int rc_wait = waitpid(rc, &info, -1);
        if (WIFEXITED(info))
            printf("Child %d terminated with status: %d\n", rc_wait, WEXITSTATUS(info));
    }
    return 0;
}

// waitpid could be useful when multiple processes are running
// but you only need a specific one