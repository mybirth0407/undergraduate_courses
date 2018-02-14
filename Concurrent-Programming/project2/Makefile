.SUFFIXES : .cpp .o
CC=g++
CFLAGS=-g -O3 --std=c++17
all: run

OBJS = ./main.o
SRCS = ./$(OBJS:.o=.cpp)
LIBNAME = -lpthread

CXXFLAGS += -I$(INCLUDE)

run: $(OBJS)
	$(CXX) -o $(BIN)run $(OBJS) $(LIBNAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@ -g

clean:
	rm -rf $(OBJS)
allclean:
	rm -rf $(OBJS) $(BIN)*
