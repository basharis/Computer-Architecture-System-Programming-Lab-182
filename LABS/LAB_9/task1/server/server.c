#include "server.h"



int main(int argc, char ** argv) {
    int bytes_recv;
    char buffer[BUFFER_SIZE];
    struct sockaddr_storage their_addr;
    socklen_t their_addr_size;
    init_server();
    printf("Server started...\n");

    for (int i = 0 ; i < argc ; i ++) {
        if (strcmp(argv[i], "-d") == 0)
            _debug = TRUE;
    }
    while ((_client_state.sock_fd = accept(_server_socket_fd, (struct sockaddr *) &their_addr, &their_addr_size)) >= 0){
        while (!_bye){
            memset(buffer, 0, BUFFER_SIZE);
            if ((bytes_recv = recv(_client_state.sock_fd, buffer, BUFFER_SIZE, 0)) < 0){
                perror("recv failed");
                exit(EXIT_FAILURE);
            }
            if (_debug) {
                fprintf(stderr, "%s | Log: %s\n", _client_state.server_addr, buffer);
            }
            if (strncmp(buffer, "hello", 5) == 0){
                handle_hello();
            }
            else if (strncmp(buffer, "bye", 3) == 0){
                handle_bye();
            }
            else if (strncmp(buffer, "ls", 2) == 0){
                handle_ls();
            }
            else {
                fprintf(stderr, "%s | ERROR: Unknown message %s\n", _client_state.client_id, buffer);
                handle_nok("unidentified");
            }
        }
        _bye = FALSE;
    }
    close(_server_socket_fd);
    return EXIT_SUCCESS;
}

void init_server(){
    int status;
    struct addrinfo hints;
    struct addrinfo * server_info;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if (gethostname(_server_addr, BUFFER_SIZE) < 0){
        perror("gethostname failed");
        exit(EXIT_FAILURE);
    }
    _client_state.server_addr = _server_addr;
    if ((status = getaddrinfo(NULL, PORT, &hints, &server_info)) != 0){
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }
    if ((_server_socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) < 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (bind(_server_socket_fd, server_info->ai_addr, server_info->ai_addrlen) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(_server_socket_fd, BACKLOG) < 0){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

void handle_hello(){
    int bytes_sent;
    if (_client_state.conn_state != IDLE){
        handle_nok("state");
        return;
    }
    _client_state.conn_state = CONNECTED;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    sprintf(_client_id, "%d" ,atoi(_client_id) + 1);
    _client_state.client_id = _client_id;
    sprintf(buffer, "hello %s", _client_state.client_id);
    if ((bytes_sent = send(_client_state.sock_fd, buffer, strlen(buffer), 0)) < 0){
        perror("send failed");
        return;
    }
    printf("Client %s connected.\n", _client_state.client_id);
}

void handle_bye(){
    char * bye = "bye";
    if (_client_state.conn_state != CONNECTED){
        handle_nok("state");
        return;
    }
    send(_client_state.sock_fd, bye, strlen(bye), 0); // DONT CARE IF CLIENT GOT THAT
    _client_state.conn_state = IDLE;
    _client_state.sock_fd = -1;
    printf("Client %s disconnected.\n", _client_state.client_id);
    _bye = TRUE;
}

void handle_ok(){
    int bytes_sent;
    char * ok = "ok ";
    if ((bytes_sent = send(_client_state.sock_fd, ok, strlen(ok), 0)) < 0){
        perror("send failed");
        exit(EXIT_FAILURE);
    }
}

void handle_nok(char* err){
    int bytes_sent;
    char nok[BUFFER_SIZE];
    memset(nok, 0, BUFFER_SIZE);
    sprintf(nok, "nok %s", err);
    if ((bytes_sent = send(_client_state.sock_fd, nok, strlen(nok), 0)) < 0){
        perror("send failed");
        return;
    }
    if (close(_client_state.sock_fd) < 0){
        perror("close failed");
        exit(EXIT_FAILURE);
    }
    _client_state.conn_state = IDLE;
    _client_state.sock_fd = -1;
}

void handle_ls(){ ////////// CONT HERE!
    int bytes_sent;
    char ls_content[LS_RESP_SIZE];
    char curr_dir[BUFFER_SIZE];
    
    handle_ok();
    getcwd(curr_dir, BUFFER_SIZE);
    strcpy(ls_content, list_dir());
    if ((bytes_sent = send(_client_state.sock_fd, ls_content, strlen(ls_content), 0)) < 0) {
        handle_nok("filesystem");
        perror("send failed");
        return;
    }
    else {
        printf("Listed files at %s\n", curr_dir);
    }
}