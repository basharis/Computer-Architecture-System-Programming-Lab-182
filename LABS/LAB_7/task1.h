#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FALSE 0
#define TRUE 1
#define BUFFER_SIZE 4096

#define SET_FILENAME 1
#define SET_SIZE 2
#define DISPLAY_FILE 3
#define MODIFY_FILE 4
#define COPY_FROM_FILE 5
#define QUIT 6

// Global variables
char* _filename = NULL;
char* _data_pointer = NULL;
int _size = 1;
void (*_action_handlers[QUIT+1]) ();
int _quit_f = FALSE;

// Handler Functions
void set_action_handlers();
void set_filename();
void set_size();
void display_file();
void modify_file();
void copy_from_file();
void quit();

// Helper Functions
void fix_endians(int len);
void display_hex(int len);
void display_dec(int len);