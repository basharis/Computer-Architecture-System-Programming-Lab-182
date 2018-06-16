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
#define R_PIPE 0
#define W_PIPE 1


void print_command_struct(cmd_line* pCmdLine) {
    if (!pCmdLine){
        printf("\n####\nEnter\n####\n\n");
        return;
    }
    printf("\n####\n");
    for (int i = 0 ; i < pCmdLine->arg_count ; i++) {
        if (pCmdLine->arguments[i] != NULL)
            printf("arguments[%i]: %s\n", 
                    i, 
                    pCmdLine->arguments[i]);
    }
    printf("arg count: %i\n"
            "input redirect: %s\n"
            "output redirect: %s\n"
            "blocking: %i\n"
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

void handle_child_process(cmd_line* pCmdLine){
    
    FILE* in_file;
    FILE* out_file;
    if (pCmdLine->input_redirect){
        close(STDIN_FD);
        if ((in_file = fopen(pCmdLine->input_redirect, "r")) == NULL){
            perror("input fopen failed");
            _exit(EXIT_FAILURE);
        }
    }
    if (pCmdLine->output_redirect){
        close(STDOUT_FD);
        if ((out_file = fopen(pCmdLine->output_redirect, "w")) == NULL){
            perror("output fopen failed");
            _exit(EXIT_FAILURE);
        }
    }
    
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) < 0) {
        perror("execvp failed");
        free_cmd_lines(pCmdLine);
        _exit(EXIT_FAILURE);
    }
    else {
        free_cmd_lines(pCmdLine);
        _exit(EXIT_SUCCESS);
    }
    
    if (in_file)
        fclose(in_file);
    if (out_file)
        fclose(out_file);
}

int handle_pipe(cmd_line* pCmdLine){
    int file_d[2];
    pid_t child_1 = -1;
    pid_t child_2 = -1;
    int status;
    
    if (pipe(file_d) < 0 ){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    
    if ((child_1 = fork()) < 0){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (child_1 == 0) {                                 /* In child 1 */
        close(STDOUT_FD);
        dup(file_d[W_PIPE]); // will be the new fd = 1
        close(file_d[W_PIPE]);
        if (pCmdLine->blocking == 0){
            perror ("unexpected token near \'|\'");
            _exit(EXIT_FAILURE);
        }
        handle_child_process(pCmdLine);
    }
    else {                                              /* In parent */
        close(file_d[W_PIPE]);
        if ((child_2 = fork()) < 0){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (child_2 == 0){                              /* In child 2 */
            close(STDIN_FD);
            dup(file_d[R_PIPE]); // will be the new fd = 0
            close(file_d[R_PIPE]);
            handle_child_process(pCmdLine->next);
        }
        else {                                          /* In parent */
            close(file_d[R_PIPE]);
            if (pCmdLine->next->blocking) {
                waitpid(child_1, &status, 0);
                waitpid(child_2, &status, 0);
            }
        }
    }
    return EXIT_SUCCESS;
}

int execute(cmd_line* pCmdLine) {
    if (!pCmdLine)                    // Empty command, user pressed enter
        return 0;
    if (!pCmdLine->next) {                  // No need for piping 
        pid_t child_process = fork();
        if (child_process < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (child_process == 0) {  // Child process
            handle_child_process(pCmdLine);
        }
        else {                          // Not child process
            if (pCmdLine->blocking) {
                int status;
                waitpid(child_process, &status, 0);
            }
        }
    }
    else                                // We pipe
        handle_pipe(pCmdLine);
    return EXIT_SUCCESS;
    
}


int main() {
    char path[PATH_MAX];
    char cmd[CMD_MAX];
    char* cmd_1;
    char* cmd_2;
    int quit = FALSE;
    cmd_line* cmd_1_struct;
    cmd_line* cmd_2_struct;
    while (!quit) {
        if (getcwd(path,PATH_MAX) < 0)
            exit(EXIT_FAILURE);
        printf("%s$ ", path);
        fgets(cmd, CMD_MAX, stdin);
        if (strcmp(cmd, "quit\n") == 0)
            quit = TRUE;
        else {
            char* r = strdup(cmd);      // strsep works on char**, not char*[size]
            cmd_1 = strsep(&r, "|");    // the first cmd, until '|'
            cmd_2 = r;                  // the second cmd is what comes after. if there's no piping, NULL
            cmd_1_struct = parse_cmd_lines(cmd_1);
            cmd_2_struct = parse_cmd_lines(cmd_2);
//            print_command_struct(cmd_1_struct);
//            print_command_struct(cmd_2_struct);
            if (cmd_1_struct)
                cmd_1_struct->next = cmd_2_struct;
            execute(cmd_1_struct);   
        }
    }
    return EXIT_SUCCESS;
}