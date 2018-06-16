PROG = sic

CC = gcc
LD = gcc

LDFLAGS = -g -Wall -no-pie
CCFLAGS = -g -Wall -c -no-pie

OBJS = sic.o

# All Targets
all: $(PROG)

# Tool invocations
# Executable "main" depends on the files...

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

# Depends on the source and header files

sic.o: sic.c
	$(CC) $(CCFLAGS) sic.c -o sic.o
	
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o $(PROG)
