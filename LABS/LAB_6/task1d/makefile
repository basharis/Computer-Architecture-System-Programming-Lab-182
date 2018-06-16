PROG = myshell

CC = gcc
LD = gcc

LDFLAGS = -g -Wall
CCFLAGS = -g -Wall -c

OBJS = myshell.o line_parser.o job_control.o

# All Targets
all: $(PROG)

# Tool invocations
# Executable "main" depends on the files...

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

# Depends on the source and header files

myshell.o: myshell.c myshell.h
	$(CC) $(CCFLAGS) myshell.c -o myshell.o

line_parser.o: line_parser.c line_parser.h
	$(CC) $(CCFLAGS) line_parser.c -o line_parser.o

job_control.o: job_control.h job_control.c
	$(CC) $(CCFLAGS) job_control.c -o job_control.o
	
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o $(PROG)
