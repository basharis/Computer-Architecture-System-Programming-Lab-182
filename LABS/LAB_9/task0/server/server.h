#ifndef SERVER_H
#define SERVER_H

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

#define BUFFER_SIZE 2048
#define CMD_MAX 2048
#define FALSE 0
#define TRUE 1
#define PORT "2018"
#define BACKLOG 10

// Global Variables
int _quit = FALSE;
int _debug = FALSE;
int _bye = FALSE;
int _server_socket_fd;

char _client_id[BUFFER_SIZE];
char _server_addr[BUFFER_SIZE];
client_state _client_state = {
    .server_addr=   NULL,
    .conn_state =   IDLE,
    .client_id  =   NULL,
    .sock_fd    =   -1
};

// Functions
int main(int argc, char ** argv);
void init_server();
void handle_hello();
void handle_bye();
void handle_ok();
void handle_nok(char* err);

#endif /* SERVER_H */

