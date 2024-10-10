#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

// Function where the system command is executed
void execArgs(char** parsed)
{
	// Forking a child
	pid_t pid = fork();

	if (pid == -1) {
		printf("\nFailed forking child..");
		return;
	} else if (pid == 0) {
		if (execvp(parsed[0], parsed) < 0) {
			printf("\nCould not execute command..");
		}
		exit(0);
	} else {
		// waiting for child to terminate
		wait(NULL);
		return;
	}
}

void begin() {
	char* clear[] = {"clear", NULL};
	execArgs(clear);
	print_rainbow_flag();
	sleep(4);
	execArgs(clear);
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
	char* pwd = getenv("PWD");
	while (1) {
		printf("%s@%s:%s$ ", user, hostname, pwd);
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

		// Print the tokens
		for (int i = 0; i < token_count; i++) {
			printf("Token %d: %s\n", i, tokens[i]);
			
		}

		execArgs(tokens);

		// Clean up
		free_tokens(tokens, token_count);
	}

	return 0;
}
