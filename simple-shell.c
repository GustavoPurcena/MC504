/*
 * Nome: Gustavo Purcena de Lima
 * RA: 198594
 * Turma: A
 * gcc -Wall -o simple-shell simple-shell.c
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_PATH 1024

void parse_input(char *input, char **args) {
    char *token = strtok(input, " \n");
    int i = 0;
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;
}

int find_command(char *command, char *path, char *full_path) {
    char *dir = strtok(path, ":");
    while (dir != NULL) {
        snprintf(full_path, MAX_PATH, "%s/%s", dir, command);
        if (access(full_path, X_OK) == 0) {
            return 1;
        }
        dir = strtok(NULL, ":");
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        exit(1);
    }

    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char full_path[MAX_PATH];
    char path[MAX_PATH];

    strncpy(path, argv[1], MAX_PATH);
    path[MAX_PATH - 1] = '\0';

    printf("simple-shell$: ");
    if (fgets(input, MAX_INPUT, stdin) == NULL) {
        perror("fgets");
        exit(1);
    }

    parse_input(input, args);

    if (args[0] == NULL) {
        fprintf(stderr, "No command entered\n");
        exit(1);
    }

    if (find_command(args[0], path, full_path)) {
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
            exit(1);
        }
    } else {
        fprintf(stderr, "Command not found: %s\n", args[0]);
        exit(1);
    }

    return 0;
}