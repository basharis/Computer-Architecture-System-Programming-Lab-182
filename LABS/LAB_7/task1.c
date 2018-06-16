#include "task1.h"

int main(int argc, char** argv) {
    int action;
    _data_pointer = (char*) malloc(BUFFER_SIZE);
    set_action_handlers();
    while (!_quit_f){
        action = 0;
        printf("Please choose action:\n"
                "1 - Set File Name\n"
                "2 - Set Size\n"
                "3 - Display File\n"
                "4 - Modify File\n"
                "5 - Copy From File\n"
                "6 - Quit\n");
        scanf("%d%*c", &action);
        if (action > 0 && action <= QUIT)
            _action_handlers[action]();
        else
            fprintf(stderr, "Invalid action.\n\n");
    } 
    return EXIT_SUCCESS;
}

void set_action_handlers() {
    _action_handlers[SET_FILENAME] = set_filename;
    _action_handlers[SET_SIZE] = set_size;
    _action_handlers[DISPLAY_FILE] = display_file;
    _action_handlers[MODIFY_FILE] = modify_file;
    _action_handlers[COPY_FROM_FILE] = copy_from_file;
    _action_handlers[QUIT] = quit;
}


// Function handlers
void set_filename() {
    if (_filename)
        free(_filename);
    _filename = (char*)malloc(FILENAME_MAX);
    printf("Please enter <filename>:\n");
    scanf("%s%*c", _filename);
    printf("Set filename successfully!\n\n");
}

void set_size() {
    int s;
    printf("Please enter <size>:\n");
    scanf("%d%*c", &s);
    if (s == 1 || s == 2 || s == 4){
        _size = s;
        printf("Set size successfully!\n\n");   
    }
    else
        fprintf(stderr, "Invalid size.\n\n");
}

void display_file() {
    int fd;
    int loc, len;   
    if (!_filename){
        fprintf(stderr, "Filename is null.\n\n");
        return;
    }
    if ((fd = open(_filename, O_RDONLY)) < 0){
        perror("open failed");
        return;
    }
    printf("Please enter <location>[HEX]  <length>[DEC]:\n");
    scanf("%x %d%*c", &loc, &len);
    if (lseek(fd, loc, SEEK_SET) < 0){
        perror("lseek failed");
        return;
    }
    if (read(fd, _data_pointer, _size*len) < 0){
        perror("read failed");
        return;
    }
    if (close(fd) < 0){
        perror("close failed");
        return;
    }
    fix_endians(len);
    display_hex(len);
    display_dec(len);
}

void modify_file() {
    int fd;
    int loc;
    int val;
    if (!_filename){
        fprintf(stderr, "Filename is null.\n\n");
        return;
    }
    if ((fd = open(_filename, O_RDWR)) < 0){
        perror("open failed");
        return;
    }
    printf("Please enter <location>[HEX]  <value>[HEX]:\n");
    scanf("%x %x%*c", &loc, &val);
    if (lseek(fd, loc, SEEK_SET) < 0){
        perror("lseek failed");
        return;
    }
    if (write(fd, &val, _size) < 0){
        perror("write failed");
        return;    
    }
    if (close(fd) < 0){
        perror("close failed");
        return;
    }
    printf("Modified file successfully!\n\n");
}

void copy_from_file() {
    char src[BUFFER_SIZE];
    char* dst = _filename;
    int src_off, dst_off;
    int fd_src, fd_dst;
    int len;
    int bytes_written;
    if (!_filename){
        fprintf(stderr, "Destination is null.\n\n");
        return;
    }
    printf("Please enter <src_file>  <src_offset>[HEX]  <dst_offset>[HEX]  <length>[DEC]:\n");
    scanf("%s %x %x %d%*c", src, &src_off, &dst_off, &len);
    char* val_s = (char*) malloc (len+1);
    if ((fd_src = open(src, O_RDONLY)) < 0){
        perror("open failed");
        return;
    }
    if ((fd_dst = open(dst, O_RDWR)) < 0){
        perror("open failed");
        return;
    }
    if (lseek(fd_src, src_off, SEEK_SET) < 0){
        perror("lseek failed");
        return;
    }
    if (lseek(fd_dst, dst_off, SEEK_SET) < 0){
        perror("lseek failed");
        return;
    }
    if (read(fd_src, val_s, len) < 0){
        perror("read failed");
        return;
    }
    if ((bytes_written = write(fd_dst, val_s, len)) < 0){
        perror("write failed");
        return;    
    }
    printf("Copied %d bytes into %x from %s to %s successfully!\n\n", bytes_written, dst_off, src, dst); 
    if (close(fd_src) < 0){
        perror("close failed");
        return;
    }
    if (close(fd_dst) < 0){
        perror("close failed");
        return;
    }
    free(val_s);
}

void quit(){
    printf("Quitting...\n\n");
    if (_filename)
        free(_filename);
    free(_data_pointer);
    _quit_f = TRUE;
}


// Helper Functions
void fix_endians(int len) {
    for (int i = 0 ; i < len ; i ++){
        for (int j = 0 ; j < _size/2 ; j ++){
            char tmp = _data_pointer[i*_size + j];
            _data_pointer[i*_size + j] = _data_pointer[i*_size + _size - j - 1];
            _data_pointer[i*_size + _size - j - 1] = tmp;
        }
    }
}

void display_hex(int len) {
    printf("\nHexadecimal Representation:\n");
//    int * a = (int*) _data_pointer;
//    for (int i = 0 ; i < len ; i ++){
//        switch(_size){
//            case 1:
//                printf("%01X", *a & 0xF);
//                a ++;
//                break;
//            case 2:
//                printf("%02X", *a & 0xFF);
//                a += 2;
//                break;
//            case 4:
//                printf("%04X", *a & 0xFFFF);
//                a += 4;
//                break;
//            default:
//                break;
//        }
//    }
    for (int i = 0 ; i < len ; i ++){
        for (int j = 0 ; j < _size ; j ++){
            printf("%02X", _data_pointer[i*_size + j] & 0xFF); // Fix non-ASCII characters
        }
        printf("\t");
    }
    printf("\n\n");
}

void display_dec(int len) {
    char num_rep[BUFFER_SIZE];
    printf("Decimal Representation:\n");
    for (int i = 0 ; i < len ; i ++){
        char* cur = num_rep;
        for (int j = 0 ; j < _size ; j ++){
            sprintf(cur, "%02X", _data_pointer[i*_size + j] & 0xFF); // Fix non-ASCII characters
            cur += 2;
        }
        long num = strtol(num_rep, NULL, 16);
        printf("%ld", num);
        printf("\t");
    }
    printf("\n\n");
}


