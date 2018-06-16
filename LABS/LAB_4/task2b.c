#include "util.h"


#define FALSE 0
#define TRUE 1

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_LSEEK 8
#define SYS_EXIT 60
#define SYS_GETDENTS 78

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define O_RDONLY 00
#define O_RDWR 02
#define O_DIRECTORY 0200000

#define DT_UNKNOWN 0
#define DT_DIR 4

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BUFFER_SIZE 131072

extern int system_call();


struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
};


int my_printf(char* st){
    int res;
    if  ((res = system_call(SYS_WRITE, STDOUT, st, simple_strlen(st))) < 0)
        system_call(SYS_EXIT, 0x55);
    return res;
}

void extract_dir(char* path, int filter, char* filt_name){
    int fd, pos = 0;
    char d_type;
    char buf[BUFFER_SIZE];
    struct linux_dirent * dirp;
    char* filename;
    if ((fd = system_call(SYS_OPEN, path, O_RDONLY | O_DIRECTORY , 0777)) < 0)
        system_call(SYS_EXIT, 0x55);
    if  (system_call(SYS_GETDENTS, fd, buf, BUFFER_SIZE) < 0)
        system_call(SYS_EXIT, 0x55);
    if  (system_call(SYS_CLOSE, fd) < 0)
        system_call(SYS_EXIT, 0x55);
        
    
    while(*(buf+pos)){
        dirp = (struct linux_dirent *) (buf + pos);
        filename = dirp->d_name;
        /* Extend the path */
        int extd_path_size = simple_strlen(path) + simple_strlen(filename) + 2; /* '/' and '\0' */
        char extended[extd_path_size];
        int i = 0;
        for (; i < simple_strlen(path) ; i++)
            extended[i] = path[i];
        extended[i] = '/';
        i++;
        int j = 0;
        for (; j < simple_strlen(filename); j++)
            extended[i+j] = filename[j];
        extended[i+j] = '\0';
        /* End extend */
        
        d_type =  *(buf + pos + dirp->d_reclen - 1);
        if (((filter && simple_strcmp(filename, filt_name) == 0) || !filter) &&
                simple_strcmp(filename, "..") != 0 && 
                simple_strcmp(filename, ".") != 0 && 
                d_type != DT_UNKNOWN){
            my_printf(extended);
            my_printf("\n");
        }

        if (simple_strcmp(filename, "..") != 0 && simple_strcmp(filename, ".") != 0){
            if (d_type == DT_DIR){
                extract_dir(extended, filter, filt_name);
            }
        }
        pos += dirp->d_reclen;
    }
}

int main(int argc, char* argv[],  char* envp[]) {
    int filter = FALSE;
    char* filt_name;
    int i;
    for (i = 1 ; i < argc ; i++){
        if (simple_strcmp(argv[i], "-n") == 0){
            filter = TRUE;
            filt_name = argv[++i];
        }
    }
    if ((filter && (simple_strcmp(".", filt_name) == 0)) || !filter)
        my_printf(".\n");
    extract_dir(".", filter, filt_name);
    return 1;
}

