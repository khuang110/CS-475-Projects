# Makefile for project 3

# Compiler
CC = g++

# Compiler flags
# -g		- debug info
# -std=c++11	- c++ version 11
CFLAGS= -std=c++11 -fopenmp

# Main file
TARGET=Source

# Output name
OUT=proj3

OBJS= $(TARGET).o helper.o animals.o

$(OUT): $(OBJS)
		$(CC) $(CFLAGS) -o $(OUT) $(OBJS)

$(TARGET).o: $(TARGET).cpp animals.h helper.h
		$(CC) $(CFLAGS) -c $(TARGET).cpp

helper.o: helper.h
		$(CC) $(CFLAGS) -c helper.cpp

animals.o: animals.h
		$(CC) $(CFLAGS) -c animals.cpp

clean:
		rm -f core $(OUT) $(OBJS)

