.SUFFIXES : .c .o
# CXX : Set compiler
CXX = gcc

# CXXFLAGS : -g : gdb use
#            -Wall : Message print when compile erorr
CXXFLAGS = -g -Wall
all: assignment3

# Set path, name Compile
OBJS = ./assignment3.o
SRCS = ./$(OBJS:.o=.c)
# New libc shared object name
# It is not necessary, because using testrun.sh script
# LIBNAME = -ldarm

# Compile options
assignment3: $(OBJS)
	$(CXX) -o $(BIN)run $(OBJS) -L$(LIB) $(LIBNAME) $(CXXFLAGS)

# make clean
clean:
	rm -rf $(OBJS)
# make allclean
allclean:
	rm -rf $(OBJS) run
