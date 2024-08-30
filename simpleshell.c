/*
 * Nome: Gustavo Purcena de Lima
 * RA: 198594
 * Turma: A
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PATH 1024
#define MAX_ARGS 64

void execute_command(char *command, char *dirs[], int dir_count) {
    char *args[MAX_ARGS];
    int arg_count = 0;
    char *token = strtok(command, " ");
    
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

    char full_path[MAX_PATH];
    int found = 0;

    for (int i = 0; i < dir_count; i++) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dirs[i], args[0]);
        if (access(full_path, X_OK) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            execv(full_path, args);
            perror("execv");
            exit(1);
        } else if (pid > 0) {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        } else {
            perror("fork");
        }
    } else {
        printf("Error: Process '%s' does not exist in specified dirs.\n", args[0]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Use with: %s <directories>\n", argv[0]);
        return 1;
    }

    char *dirs[MAX_ARGS];
    int dir_count = 0;
    char *token = strtok(argv[1], ":");

    while (token != NULL && dir_count < MAX_ARGS) {
        dirs[dir_count++] = token;
        token = strtok(NULL, ":");
    }

    char command[MAX_PATH];
    printf("simple-shell$: ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
        return 0;
    }
    command[strcspn(command, "\n")] = 0;

    execute_command(command, dirs, dir_count);

    return 0;
}
