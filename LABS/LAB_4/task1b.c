/* DO NOT use standard libs */

#include "util.h"

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_LSEEK 8
#define SYS_EXIT 60

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define O_RDWR 2

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define NAME_OFFSET 0x291

extern int system_call();

/* for testing purposes */
int my_printf(char* st){
    return system_call(SYS_WRITE, STDOUT, st, simple_strlen(st));
}

void fix_name(char* original, char* str_to_fill){
    int i;
    int len = simple_strlen(original);
    for (i = 0 ; i<len; i++){
        str_to_fill[i] = original[i];
    }
    str_to_fill[i] = '.';
    str_to_fill[i+1] = '\n';
    str_to_fill[i+2] = '\0';
}

int main(int argc, char* argv[],  char* envp[]) {
    char * filename = argv[1];
    char * replacement = argv[2];
    int repl_len = simple_strlen(replacement);
    char replacement_fixed[repl_len + 3]; /* for ".\n\0" */
    fix_name(replacement, replacement_fixed);
    int fd;
    if ((fd = system_call(SYS_OPEN, filename, O_RDWR, 0777)) < 0)
        system_call(SYS_EXIT, 0x55);
    if (system_call(SYS_LSEEK, fd, NAME_OFFSET, SEEK_SET) < 0)
        system_call(SYS_EXIT, 0x55);
    if ((system_call(SYS_WRITE, fd, replacement_fixed, simple_strlen(replacement_fixed)+1)) < 0) /* for '/0' */
        system_call(SYS_EXIT, 0x55);
    if (system_call(SYS_CLOSE, fd) < 0)
        system_call(SYS_EXIT, 0x55);
    return 1;
}

