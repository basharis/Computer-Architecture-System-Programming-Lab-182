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

#define BUFFER_SIZE 8192

extern int system_call();


struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
};
typedef struct flags{
    int     filter;           /* Flag "-n" , turns on also for "-e" */
    int     exec;             /* Flag "-e" */
    char*   filt_name;        /* "name" in -n <name> and -e <name> <command> */
    char*   command;          /* "command" in -e <name> <command> */
} flags;

int my_printf(char* st){
    int res;
    if  ((res = system_call(SYS_WRITE, STDOUT, st, simple_strlen(st))) < 0)
        system_call(SYS_EXIT, 0x55);
    return res;
}
int open_dir(char* path){
    int fd;
    if ((fd = system_call(SYS_OPEN, path, O_RDONLY | O_DIRECTORY , 0777)) < 0)
        system_call(SYS_EXIT, 0x55);
    return fd;
}
void my_getdents(int fd, char* buf){
    if  (system_call(SYS_GETDENTS, fd, buf, BUFFER_SIZE) < 0)
        system_call(SYS_EXIT, 0x55);
}
void close_dir(int fd){
    if  (system_call(SYS_CLOSE, fd) < 0)
        system_call(SYS_EXIT, 0x55);
}

void extract_dir(char* path, flags* fl){
    int fd, pos = 0;
    char d_type;
    char buf[BUFFER_SIZE];
    struct linux_dirent * dirp;
    char* filename;
    fd = open_dir(path);
    my_getdents(fd,buf);
    close_dir(fd);
    
    
    while(*(buf+pos)){
        dirp = (struct linux_dirent *) (buf + pos);
        filename = dirp->d_name;
        
        /* Extend the path */
        int extd_path_size = simple_strlen(path) + simple_strlen(filename) + 2;  /* '/' and '\0' */
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
        if (((fl->filter && simple_strcmp(filename, fl->filt_name) == 0) || !fl->filter) &&
                simple_strcmp(filename, "..") != 0 && 
                simple_strcmp(filename, ".") != 0 && 
                d_type != DT_UNKNOWN){
            if (!fl->exec){
                my_printf(extended);
                my_printf("\n");
            }
            else{
                /* Create the full command <command> <full_path> */
                char full_cmd[simple_strlen(fl->command) + simple_strlen(extended) + 2]; /* for space and \0 */
                int idx = 0, s = 0;
                for (; s < simple_strlen(fl->command); s++)
                    full_cmd[idx++] = fl->command[s];
                full_cmd[idx++] = ' ';
                for (s = 0 ; s < simple_strlen(extended) ; s++)
                    full_cmd[idx++] = extended[s];
                full_cmd[idx] = '\0';
                simple_system(full_cmd);
            }
        }

        if (simple_strcmp(filename, "..") != 0 && simple_strcmp(filename, ".") != 0 && d_type == DT_DIR)
            extract_dir(extended, fl);
        pos += dirp->d_reclen;
    }
}

void drill_down(flags fl){
    if (!fl.exec && ((fl.filter && (simple_strcmp(".", fl.filt_name) == 0)) || !fl.filter))
        my_printf(".\n");
    extract_dir(".", &fl);
}

int main(int argc, char* argv[],  char* envp[]) {
    flags fl;
    fl.filter = FALSE;
    fl.exec = FALSE;
    int i;
    for (i = 1 ; i < argc ; i++){
        if (simple_strcmp(argv[i], "-n") == 0){
            fl.filter = TRUE;
            fl.filt_name = argv[++i];
            drill_down(fl);
        }
        if (simple_strcmp(argv[i], "-e") == 0){
            fl.exec = TRUE;
            fl.filter = TRUE;
            fl.filt_name = argv[++i];
            
            /* Command could be multiple words, ls -l for example */
            int command_length = 0;
            int counter = ++i;
            for (; counter < argc ; counter++)
                command_length += simple_strlen(argv[counter]) + 1; /* for space and finally \0 */
            char command[command_length];
            int index_in_cmd = 0;
            int j;
            for (; i < argc; i++){
                int cmd_part_len = simple_strlen(argv[i]);
                for (j = 0 ; j < cmd_part_len; j++)
                    command[index_in_cmd++] = argv[i][j];
                command[index_in_cmd++] = ' ';
            }
            command[index_in_cmd] = '\0';
            fl.command = command;
            drill_down(fl);
        }
    }
    if (!fl.exec && !fl.filter){
        drill_down(fl);
    }
    return 1;
}
