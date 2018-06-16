#include "client.h"


int main(int argc, char ** argv) {
    char cmd_line_char[CMD_MAX];
    cmd_line * cmd;
    
    for (int i = 0 ; i < argc ; i ++) {
        if (strcmp(argv[i], "-d") == 0)
            _debug = TRUE;
    }
    
    while (!_quit) {
        printf("server:%s$> ", _client_state.server_addr);
        fgets(cmd_line_char, CMD_MAX, stdin);
        if (strcmp(cmd_line_char, "q\n") == 0 || strcmp(cmd_line_char, "quit\n") == 0){
            printf("Quitting...\n");
            _quit = TRUE;
            break;
        }
        cmd = parse_cmd_lines(cmd_line_char);
        exec(cmd);
    }
    return EXIT_SUCCESS;
}

int exec(cmd_line * cmd) {
    if (!cmd) // ENTER
        return 0;
    char * cmd_name = cmd->arguments[0];
    if (strcmp(cmd_name, "conn") == 0 || strcmp(cmd_name, "c") == 0 || strcmp(cmd_name, "connect") == 0){ 
        if (_client_state.conn_state == IDLE)
            handle_conn(cmd);
        else {
            fprintf(stderr, "Connection state not IDLE.\n");
            return -2;
        }
    }
    else if (strcmp(cmd_name, "bye") == 0 || strcmp(cmd_name, "b") == 0) 
        if (_client_state.conn_state == CONNECTED)
            handle_bye(cmd);
        else {
            fprintf(stderr, "Connection state not CONNECTED.\n");
            return -2;
        }
    else
        fprintf(stderr, "Invalid command, please try again.\n");
    free_cmd_lines(cmd);
    return EXIT_SUCCESS;
}

void handle_conn(cmd_line * cmd) {
    char buffer[BUFFER_SIZE];
    char * hello = "hello";
    int status, bytes_sent, bytes_recv;
    struct addrinfo hints;
    struct addrinfo * server_info;
    
    memset(&hints, 0, sizeof(hints));
    memset(buffer, 0, BUFFER_SIZE);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(cmd->arguments[1], PORT, &hints, &server_info)) != 0){
        perror("getaddrinfo failed");
        return;
    }
    if ((_client_state.sock_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) < 0){
        perror("socket failed");
        return;
    }
    connect(_client_state.sock_fd, server_info->ai_addr, server_info->ai_addrlen);
    if ((bytes_sent = send(_client_state.sock_fd, hello, strlen(hello), 0)) < 0){
        perror("send failed");
        return;
    }
    _client_state.conn_state = CONNECTING;
    if ((bytes_recv = recv(_client_state.sock_fd, buffer, BUFFER_SIZE, 0)) < 0){
        perror("recv failed");
        return;
    }
    strcpy(_server_addr, cmd->arguments[1]);
    _client_state.server_addr = _server_addr;
    _client_state.conn_state = CONNECTED;
    _client_state.client_id = &buffer[strlen("hello") + 1];
    if (_debug) {
        fprintf(stderr, "%s | Log: %s\n", _client_state.server_addr, buffer);
    }
    freeaddrinfo(server_info);
}

void handle_bye() {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_recv, bytes_sent;
    char * bye = "bye";
    if ((bytes_sent = send(_client_state.sock_fd, bye, strlen(bye), 0)) < 0){
        perror("send failed");
        return;
    }
    if (_debug) {
        if ((bytes_recv = recv(_client_state.sock_fd, buffer, BUFFER_SIZE, 0)) < 0){ // DONT CARE IF THE SERVER RESPONDS
            perror("recv failed");
            return;
        }
        fprintf(stderr, "%s | Log: %s\n", _client_state.server_addr, buffer);
    }
    if (close(_client_state.sock_fd) < 0){
        perror("close failed");
        exit(EXIT_FAILURE);
    }
    _client_state.server_addr = "(nil)";
    _client_state.conn_state = IDLE;
    _client_state.client_id = NULL;
    _client_state.sock_fd = -1;
}


/**
 * DEBUG FUNCTION
 * Printing @cmdline_ptr, nicely formatted.
 */
void print_command_struct(cmd_line * cmdline_ptr) {
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