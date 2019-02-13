CC = gcc
SRCS = $(shell find *.c)
OBJS = $(SRCS:.c=.o)

LIBS = 
CFLAGS = -std=c99 -s -Os -Wall -g -pg
LDFLAGS = -pg
EXE = test

all: $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(LDFLAGS) 
	
.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)
	
.PHONY: clean profile

profile:
	gprof -b $(EXE).exe gmon.out -p
	
clean:
	-rm $(OBJS)