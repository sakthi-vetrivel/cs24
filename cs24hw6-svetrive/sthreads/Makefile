#
# Commands and arguments
#

CC = gcc
RM = rm

CFLAGS = -Wall -g -std=c99 -pedantic
ASFLAGS = -g


all: test


# The simple test program
test: sthread.o queue.o glue.o test.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


# pseudo-target to clean up
clean:
	$(RM) -f *.o core* *~ test


.PHONY: all clean


# Dependencies
sthread.c: sthread.h queue.h
queue.c: queue.h sthread.h
test.c: sthread.h

