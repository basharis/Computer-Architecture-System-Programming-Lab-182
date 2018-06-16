#include "linux/limits.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "line_parser.h"

#define CMD_MAX 2048
#define TRUE 1
#define FALSE 0
#define STDIN_FD 0
#define STDOUT_FD 1

void print_command_struct(cmd_line* pCmdLine) {
    if (!pCmdLine) {
        printf("\n####\nEnter\n####\n\n");
        return;
    }
    printf("\n####\n");
    for (int i = 0; i < pCmdLine->arg_count; i++) {
        if (pCmdLine->arguments[i] != NULL)
            printf("arguments[%i]: %s\n",
                i,
                pCmdLine->arguments[i]);
    }
    printf("arg count: %i\n"
            "input redirect: %s\n"
            "output redirect: %s\n"
            "blocking: %c\n"
            "idx: %i\n",
            pCmdLine->arg_count,
            pCmdLine->input_redirect,
            pCmdLine->output_redirect,
            pCmdLine->blocking,
            pCmdLine->idx);
    if (pCmdLine->next)
        print_command_struct(pCmdLine->next);
    printf("####\n\n");
}

void handle_child_process(cmd_line* pCmdLine) {

    FILE* in_file;
    FILE* out_file;
    if (pCmdLine->input_redirect) {
        close(STDIN_FD);
        if ((in_file = fopen(pCmdLine->input_redirect, "r")) == NULL) {
            perror("input fopen failed");
            _exit(EXIT_FAILURE);
        }
    }
    if (pCmdLine->output_redirect) {
        close(STDOUT_FD);
        if ((out_file = fopen(pCmdLine->output_redirect, "w")) == NULL) {
            perror("output fopen failed");
            _exit(EXIT_FAILURE);
        }
    }

    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) < 0) {
        perror("execvp failed");
        free_cmd_lines(pCmdLine);
        _exit(EXIT_FAILURE);
    } else {
        free_cmd_lines(pCmdLine);
        _exit(EXIT_SUCCESS);
    }

    if (in_file)
        fclose(in_file);
    if (out_file)
        fclose(out_file);
}

int execute(cmd_line* pCmdLine) {
    if (!pCmdLine) /* Empty command, user pressed enter */
        return 0;
    pid_t child_process = fork();
    if (child_process == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (child_process == 0) { /* Child process */
        handle_child_process(pCmdLine);
    } else { /* Not child process */
        if (pCmdLine->blocking) {
            int status;
            waitpid(child_process, &status, 0);
        }
    }
    return EXIT_SUCCESS;

}

int main() {
    char path[PATH_MAX];
    char cmd[CMD_MAX];
    int quit = FALSE;
    cmd_line* cmd_struct;
    while (!quit) {
        if (getcwd(path, PATH_MAX) < 0)
            exit(EXIT_FAILURE);
        printf("%s$ ", path);
        fgets(cmd, CMD_MAX, stdin);
        if (strcmp(cmd, "quit\n") == 0) {
            quit = TRUE;
        } else {
            cmd_struct = parse_cmd_lines(cmd);
//             print_command_struct(cmd_struct);
            execute(cmd_struct);
        }
    }
    return EXIT_SUCCESS;
}