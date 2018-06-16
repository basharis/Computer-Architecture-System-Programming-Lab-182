#include "linux/limits.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0
#define STDIN_FD 0
#define STDOUT_FD 1




int main() {
    int file_d[2];
    pid_t child_1 = -1;
    pid_t child_2 = -1;
    int status;
    char* arg1[3] = {"ls", "-l", 0};
    char* arg2[4] = {"tail", "-n", "2", 0};
    
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
        dup(file_d[1]); // will be the new fd = 1
        close(file_d[1]);
        if (execvp(arg1[0], arg1) < 0) {
            perror("execvp failed");
            _exit(EXIT_FAILURE);
        }
    }
    else {                                              /* In parent */
        close(file_d[1]);
        if ((child_2 = fork()) < 0){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (child_2 == 0){                              /* In child 2 */
            close(STDIN_FD);
            dup(file_d[0]); // will be the new fd = 0
            close(file_d[0]);
            if (execvp(arg2[0], arg2) < 0) {
                perror("execvp failed");
                _exit(EXIT_FAILURE);
            }
        }
        else {                                          /* In parent */
            close(file_d[0]);
            waitpid(child_1, &status, 0);
            waitpid(child_2, &status, 0);
        }
    }
    return EXIT_SUCCESS;
}