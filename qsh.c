#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 1024
#define MAX_TOKENS 100

void print_rainbow_flag() {
    const char *rainbow_colors[] = {
        "\033[31m████████████████████\033[0m",  // Red
        "\033[33m████████████████████\033[0m",  // Orange
        "\033[32m████████████████████\033[0m",  // Yellow
        "\033[34m████████████████████\033[0m",  // Green
        "\033[36m████████████████████\033[0m",  // Blue
        "\033[35m████████████████████\033[0m"   // Purple
    };

    for (int i = 0; i < 6; i++) {
        printf("%s\n", rainbow_colors[i]);
    }
}

// Function to execute a command with redirection and pipes
void exec_command(char **parsed, int token_count) {
    if (token_count == 0) return; // No command

    // Handle 'cd' command separately
    if (strcmp(parsed[0], "cd") == 0) {
        if (token_count > 1) {
            if (chdir(parsed[1]) != 0) {
                perror("cd failed");
            }
        } else {
            fprintf(stderr, "cd: missing argument\n");
        }
        return;
    }

    // Handle 'exit' command
    if (strcmp(parsed[0], "exit") == 0) {
        exit(0);
    }

    int pipe_count = 0;

    // Count number of pipes
    for (int i = 0; i < token_count; i++) {
        if (strcmp(parsed[i], "|") == 0) {
            pipe_count++;
        }
    }

    int pipefd[2 * pipe_count]; // Array to hold pipe file descriptors
    for (int i = 0; i < pipe_count; i++) {
        if (pipe(pipefd + i * 2) == -1) {
            perror("Pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    int j = 0; // To track position in parsed command
    for (int i = 0; i <= pipe_count; i++) {
        // Find the command to execute
        char *command[MAX_TOKENS];
        int cmd_index = 0;

        while (j < token_count && strcmp(parsed[j], "|") != 0) {
            command[cmd_index++] = parsed[j++];
        }
        command[cmd_index] = NULL; // Null-terminate the command array

        // Fork the child process
        pid_t pid = fork();
        if (pid == 0) { // Child process
            // Handle input/output redirection for the command
            for (int k = 0; k < cmd_index; k++) {
                if (strcmp(command[k], ">") == 0) {
                    int fd = open(command[k + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1) {
                        perror("Error opening file for output redirection");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    command[k] = NULL; // Terminate command
                    break;
                } else if (strcmp(command[k], "<") == 0) {
                    int fd = open(command[k + 1], O_RDONLY);
                    if (fd == -1) {
                        perror("Error opening file for input redirection");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    command[k] = NULL; // Terminate command
                    break;
                }
            }

            // Set up pipes
            if (i < pipe_count) {
                dup2(pipefd[i * 2 + 1], STDOUT_FILENO); // Output to the next pipe
            }
            if (i > 0) {
                dup2(pipefd[(i - 1) * 2], STDIN_FILENO); // Input from the previous pipe
            }

            // Close all pipe file descriptors
            for (int k = 0; k < 2 * pipe_count; k++) {
                close(pipefd[k]);
            }

            // Execute the command
            if (execvp(command[0], command) < 0) {
                perror("Could not execute command");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        j++; // Move past the pipe
    }

    // Close all pipe file descriptors in the parent
    for (int k = 0; k < 2 * pipe_count; k++) {
        close(pipefd[k]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i <= pipe_count; i++) {
        wait(NULL);
    }
}

void begin() {
    char* clear[] = {"clear", NULL};
    exec_command(clear, 1);
    print_rainbow_flag();
    sleep(4);
    exec_command(clear, 1);
}

char *readline() {
    char *line = malloc(MAX_LINE_LENGTH);
    if (!line) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    if (fgets(line, MAX_LINE_LENGTH, stdin) == NULL) {
        free(line);
        return NULL; // Return NULL on EOF or error
    }

    // Remove newline character if present
    line[strcspn(line, "\n")] = '\0';
    return line;
}

char **parse_line(char *line, int *token_count) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (!tokens) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    *token_count = 0;
    char *token = strtok(line, " ");

    while (token != NULL && *token_count < MAX_TOKENS) {
        tokens[*token_count] = strdup(token); // Duplicate the token
        if (!tokens[*token_count]) {
            fprintf(stderr, "Memory allocation failed\n");
            // Free previously allocated tokens
            for (int i = 0; i < *token_count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return NULL;
        }
        (*token_count)++;
        token = strtok(NULL, " ");
    }

    return tokens;
}

void free_tokens(char **tokens, int token_count) {
    for (int i = 0; i < token_count; i++) {
        free(tokens[i]); // Free each duplicated token
    }
    free(tokens); // Free the array itself
}

int main() {
    begin();
    char* user = getenv("USER");
    char* hostname = getenv("HOSTNAME");
    while (1) {
        printf("%s@%s$ ", user, hostname);
        char *line = readline();
        if (!line) {
            return 1; // Handle read error
        }

        int token_count;
        char **tokens = parse_line(line, &token_count);
        free(line); // Free the input line after parsing

        if (!tokens) {
            return 1; // Handle parse error
        }

        // Execute the command with pipes and redirection
        exec_command(tokens, token_count);

        // Clean up
        free_tokens(tokens, token_count);
    }

    return 0;
}
