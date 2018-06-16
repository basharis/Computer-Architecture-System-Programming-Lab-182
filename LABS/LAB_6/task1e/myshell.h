#ifndef MYSHELL_H
#define MYSHELL_H

#include "linux/limits.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "line_parser.h"
#include "job_control.h"

#define CMD_MAX 2048
#define FALSE 0
#define TRUE 1
#define RD_END 0
#define WR_END 1

int main(int argc, char** argv);
void init_shell(struct termios* ter_att);
void handle_signal(int sig);
int execute(cmd_line* cmdline_ptr, job* new_job, pid_t shell_pgid, job** job_list, struct termios* ter_att);
int cont_execute(cmd_line* cmdline_ptr, int read_pipe[2], int write_pipe[2], job* new_job, pid_t pgid, pid_t shell_pgid, job** job_list, struct termios* ter_att);
void do_pipes_child(int read_pipe[2], int write_pipe[2]);
void do_pipes_parent(int read_pipe[2], int write_pipe[2]);
int execute_child(cmd_line* cmdline_ptr);
void print_command_struct(cmd_line* cmdline_ptr);

#endif /* MYSHELL_H */

