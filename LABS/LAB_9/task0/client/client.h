#ifndef CLIENT_H
#define CLIENT_H

#include "linux/limits.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include "line_parser.h"
#include "common.h"

#define BUFFER_SIZE 4096
#define CMD_MAX 2048
#define FALSE 0
#define TRUE 1
#define PORT "2018"

// Global Variables
int _quit = FALSE;
int _debug = FALSE;

char _server_addr[BUFFER_SIZE];
client_state _client_state = {
    .server_addr=   "(nil)",
    .conn_state =   IDLE,
    .client_id  =   NULL,
    .sock_fd    =   -1
};

// Functions
int main(int argc, char ** argv);
int exec(cmd_line * cmd);
void handle_conn(cmd_line * cmd);
void handle_bye();

void print_command_struct(cmd_line * cmdline_ptr);

#endif /* CLIENT_H */

