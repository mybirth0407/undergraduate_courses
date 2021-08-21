.SUFFIXES : .cpp .o

CC=g++
CFLAGS=-g -O3 --std=c++11
all: run

OBJS = ./main.o
SRCS = ./$(OBJS:.o=.cpp)
BIN = ./
LIBNAME = -lpthread

run: $(OBJS)
	$(CC) -o $(BIN)run $(OBJS) $(LIBNAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf $(OBJS)
allclean:
	rm -rf $(OBJS) $(BIN)*
