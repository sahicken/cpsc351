/* 3. Write a program that calls fork() and then calls some form of exec() to run the program
/bin/ls. See if you can try all of the variants of exec(), including (on Linux) execl(),
execle(), execlp(), execv(), execvp(), and execvpe(). Why do you think
there are so many variants of the same basic call? */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, const char* argv[]) {
    char* pathname = "/bin/ls"; char* file = "ls";
    char* const env[] = { "FOLDER='..'", NULL };
    char* const args[] = { strdup("-lAh"), strdup("/"), NULL };
    char* const eArgs[] = { strdup("-lAh"), strdup("$FOLDER"), NULL };

    int rc = fork();
    if (rc == 0) {
        execl(pathname, args[0], NULL);
    } else {
        rc = fork();
    } if (rc == 0) {
        execlp(file, args[0], NULL);
    } else {
        rc = fork();
    } if (rc == 0) {
        execv(pathname, args);
    } else {
        rc = fork();
    } if (rc == 0) {
        execle(pathname, file, eArgs[1], env);
    } else {
        rc = fork();
    } if (rc == 0) {
        execvp(file, args);
    } else {
        rc = fork();
    } if (rc == 0) {
        //execvpe(file, args, env, NULL);
    }
    return 0;
}"

// THE PURPOSE OF ALL THE DIFFERENT VERSIONS OF EXEC
// IS TO PROVIDE ENVIRONMENTAL VARIABLES, PATHS, OR
// TO ALLOW "LISTS" OF "STRINGS" VS JUST ONE STRING