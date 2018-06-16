#include "myshell.h"

/**
 * Shell command line logic.
 */
int main(int argc, char** argv) {
    char path[PATH_MAX];
    char cmd[CMD_MAX];
    int quit = FALSE;
    cmd_line* cmd_struct;
    job* job_list;
    job* new_job;
    struct termios ter_att;
    
    init_shell(&ter_att);

    while (!quit) {
        tcsetattr(STDIN_FILENO, TCSANOW, &ter_att); // Return the saved terminal attributes
        if (getcwd(path,PATH_MAX) < 0){
            perror("getcwd failed");
            exit(EXIT_FAILURE);
        }
        printf("%s$ ", path);
        fgets(cmd, CMD_MAX, stdin);
        if (strcmp(cmd, "quit\n") == 0 || strcmp(cmd, "q\n") == 0)
            quit = TRUE;
        else if (strcmp(cmd, "\n") == 0)             // ENTER
            continue;
        else if (strncmp(cmd, "bg ", 3) == 0){
            job* cont_job = find_job_by_index(job_list, cmd[3] - '0');
            if (cont_job)
                run_job_in_background(cont_job ,TRUE);
        }
        else if (strncmp(cmd,"fg ", 3) == 0){
            job* cont_job = find_job_by_index(job_list, cmd[3] - '0');
            if (cont_job)
                run_job_in_foreground(&job_list, cont_job, TRUE, &ter_att, getpid());
        }
        else if (strcmp(cmd, "jobs\n") == 0)
            print_jobs(&job_list);
        else {
            new_job = add_job(&job_list, cmd);
            new_job->status = RUNNING; 
            cmd_struct = parse_cmd_lines(cmd);
            execute(cmd_struct, new_job, getpid(), &job_list, &ter_att);
        }
    }
    free_job_list(&job_list);
    return EXIT_SUCCESS;
}

/**
 * Initializes the shell.
 * Sets shell pgid, sets it to be terminal foreground, 
 *  and saves its attributes
 */
void init_shell(struct termios* ter_att){
    if (setpgid(getpid(), getpid()) < 0) {          // Set the PGID of the shell to the PID of the shell
        perror("setpgid failed");
        exit(EXIT_FAILURE);
    }
    if (tcsetpgrp(STDIN_FILENO, getpid()) < 0) {    // Set the PGID of the shell as the terminal foreground
        perror("tcsetgrp failed");
        exit(EXIT_FAILURE);
    }
    if (tcgetattr(STDIN_FILENO, ter_att) != 0) {    // Save termios' attributes
        perror("tcgetattr failed");
        exit(EXIT_FAILURE);
    }
    // Handle signals
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGQUIT, handle_signal);
    signal(SIGCHLD, handle_signal);
}

/**
 * Deal with signals.
 * Ignore signals to make sure they reach foreground
 *  child process rather than the shell.
 */
void handle_signal(int sig){
    char* sig_str = strsignal(sig);
    switch (sig){
        case SIGQUIT:
//            signal(sig, SIG_DFL);
//            printf("\nCaught Signal: %s\n", sig_str);
            break;
        case SIGCHLD:
//            signal(sig, SIG_DFL);
//            printf("\nCaught Signal: %s\n", sig_str);
            break;
        default:
            signal(sig, SIG_IGN);
            printf("\nSignal ignored: %s\n", sig_str);
            break;
    }
}

/**
 * Wrapper function for execution,
 *  handles first invokation.
 */
int execute(cmd_line* cmdline_ptr, job* j, pid_t shell_pgid, job** job_list, struct termios* ter_att){
    if (!cmdline_ptr)
        return EXIT_SUCCESS;
    else if (cmdline_ptr->next){
        int file_d[2];
        if (pipe(file_d) < 0 ){
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
        cont_execute(cmdline_ptr, NULL, file_d, j, -1, shell_pgid ,job_list, ter_att);
    }
    else
        cont_execute(cmdline_ptr, NULL, NULL, j, -1, shell_pgid, job_list, ter_att);
    return EXIT_SUCCESS;
}

/**
 * The main recursive fuction for handling pipes.
 * Receives left pipe (reading from previous process) 
 *  and right pipe (writing into next process).
 */
int cont_execute(cmd_line* cmdline_ptr, int read_pipe[2], int write_pipe[2], job* j, pid_t pgid, pid_t shell_pgid, job** job_list, struct termios* ter_att) {
    pid_t child_process;
    cmd_line* last_cmd = cmdline_ptr;
    char blocking;
   
    while (last_cmd->next) { last_cmd = last_cmd->next; }
    blocking = last_cmd->blocking;   
    if ((child_process = fork()) < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (child_process == 0) {              // In child process
        if (pgid < 0){
            setpgid(getpid(), getpid());
            pgid = getpgid(getpid());
        }
        else
            setpgid(getpid(), pgid);
        if (!j->pgid)
            j->pgid = pgid;
        
//        printf("\nPID: %d\nPGID: %d\nJOB PGID: %d\nPROCESS PGID: %d", getpid(), pgid, j->pgid, getpgid(getpid()));
        /* Restore signals to default */
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        do_pipes_child(read_pipe, write_pipe);
        execute_child(cmdline_ptr);
    }
    else {                                      // In parent process
        if (pgid < 0){
            setpgid(child_process, child_process);
            pgid = child_process;
        }
        else
            setpgid(child_process, pgid);
        if (!j->pgid)
            j->pgid = pgid;
        
        do_pipes_parent(read_pipe, write_pipe);
        if (cmdline_ptr->next){
            if (cmdline_ptr->next->next) {
                int file_d[2];
                if (pipe(file_d) < 0 ){
                    perror("pipe failed");
                    exit(EXIT_FAILURE);
                }
                cont_execute(cmdline_ptr->next, write_pipe, file_d, j, pgid, shell_pgid, job_list, ter_att);
            }
            else
                cont_execute(cmdline_ptr->next, write_pipe, NULL, j, pgid, shell_pgid, job_list, ter_att);
        }
        if (blocking)
            run_job_in_foreground(job_list,j, FALSE, ter_att, shell_pgid);
        else
            run_job_in_background(j, FALSE);
    }
    return EXIT_SUCCESS;   
}

/**
 * 
 * @param read_pipe
 * @param write_pipe
 */
void do_pipes_child(int read_pipe[2], int write_pipe[2]){
    if (write_pipe) {
        close(STDOUT_FILENO);
        dup(write_pipe[WR_END]);            // Will be the new fd = 1
        close(write_pipe[WR_END]);
    }
    if (read_pipe) {
        close(STDIN_FILENO);
        dup(read_pipe[RD_END]);             // Will be the new fd = 0
        close(read_pipe[RD_END]);
    }
}

/**
 * 
 * @param read_pipe
 * @param write_pipe
 */
void do_pipes_parent(int read_pipe[2], int write_pipe[2]){
    if (write_pipe)
        close(write_pipe[WR_END]);
    if (read_pipe)
        close(read_pipe[RD_END]);
}


/**
 * Execute the command.
 * If input/output redirect are required, this function
 *  takes care of that too.
 */
int execute_child(cmd_line* cmdline_ptr) {
    FILE* in_file = NULL;
    FILE* out_file = NULL;
    char path[PATH_MAX];
    getcwd(path,PATH_MAX);
    
    if (cmdline_ptr->input_redirect) {
        close(STDIN_FILENO);
        if ((in_file = fopen(cmdline_ptr->input_redirect, "r")) == NULL) {
            perror("input fopen failed");
            _exit(EXIT_FAILURE);
        }
    }
    if (cmdline_ptr->output_redirect) {
        close(STDOUT_FILENO);
        if ((out_file = fopen(cmdline_ptr->output_redirect, "w")) == NULL) {
            perror("output fopen failed");
            _exit(EXIT_FAILURE);
        }
    }
    
    if (execvp(cmdline_ptr->arguments[0], cmdline_ptr->arguments) < 0 && execv(path, cmdline_ptr->arguments)) {
        perror("execvp/execv failed");
        free_cmd_lines(cmdline_ptr);
        _exit(EXIT_FAILURE);
    }
    else {
        free_cmd_lines(cmdline_ptr);
        if (in_file)
            fclose(in_file);
        if (out_file)
            fclose(out_file);
        _exit(EXIT_SUCCESS);
    }
    
    return EXIT_SUCCESS;
}


/**
 * DEBUG FUNCTION
 * Printing @cmdline_ptr, nicely formatted.
 */
void print_command_struct(cmd_line* cmdline_ptr) {
    if (!cmdline_ptr){
        printf("\n####\nEnter\n####\n\n");
        return;
    }
    printf("\n####\n");
    for (int i = 0 ; i < cmdline_ptr->arg_count ; i++) {
        if (cmdline_ptr->arguments[i] != NULL)
            printf("arguments[%i]: %s\n", 
                    i, 
                    cmdline_ptr->arguments[i]);
    }
    printf("arg count: %i\n"
            "input redirect: %s\n"
            "output redirect: %s\n"
            "blocking: %i\n"
            "idx: %i\n", 
            cmdline_ptr->arg_count, 
            cmdline_ptr->input_redirect, 
            cmdline_ptr->output_redirect,
            cmdline_ptr->blocking,
            cmdline_ptr->idx);
    printf("####\n\n");
    if (cmdline_ptr->next)
        print_command_struct(cmdline_ptr->next);
}