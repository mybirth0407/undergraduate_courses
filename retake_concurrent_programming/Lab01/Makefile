# Compiler and Compile options.
CC = g++
CXXFLAGS = -g -Wall -std=c++11

# Macros specifying path for compile.
SRCS := prac_pthread.cpp
OBJS := $(SRCS:.cc=.o)
INC = ./include/
LIB = ./lib/ -lpthread

# Pre-Processor.
CCFLAGS += -I$(INC)

# Compile command.
TARGET = prac_pthread
$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(CCFLAGS) -o $(TARGET) $(OBJS) -L$(LIB)

# Delete binary & object files.
clean:
	rm $(TARGET) $(OBJS)
