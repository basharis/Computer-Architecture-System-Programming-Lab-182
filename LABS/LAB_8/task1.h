#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>

#define FALSE 0
#define TRUE 1
#define BUFFER_SIZE 4096
#define MACRO_NAME(x) #x

struct action {
    char* name;
    void (*func)();
};

// Handler Functions
void examine_elf_file();
void print_section_names();
void quit();

// Helper Functions
void open_file();
char* type_to_str (int type);

// Global variables
int _quit_f = FALSE;
void * _map_start = NULL;
struct stat _fd_stat;
char _filename[BUFFER_SIZE];
struct action _menu[] = {
        {  NULL,                       NULL               },
        {  "Examine ELF File",         examine_elf_file   },
        {  "Print Section Names",      print_section_names},
        {  "Quit",                     quit               }
};