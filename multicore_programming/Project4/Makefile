.SUFFIXES : .cc .o

# 컴파일러 지정
#CC = gcc
CC = g++

# CFLAGS: -g: 프로그램 에러 시 디버깅용 파일 생성 (core)
#		  -Wall: 컴파일 에러 시 메세지 출력
#		  -ansi: 표준 ansi C 문법 사용
CFLAGS = -g -Wall -ansi
 all: hw # Question

# 아래의 매크로들은 컴파일 시 경로지정을 위해 필요
#OBJS = ./src/example.o
#OBJS = ./src/pthread1.o
#OBJS = ./src/pthread2.o
#OBJS = ./src/pthread3.o
#OBJS = ./src/project1.o
#OBJS = ./src/project2.o
OBJS = ./src/project4.o

SRCS = ./src/$(OBJS:.o=.c)
BIN = ./bin/
LIB = ./lib
INCLUDE = ./include
#LIBNAME = -lhello
LIBNAME = -lpthread -lm

# CFALGS에 I옵션 추가를 통해 헤더파일 위치 지정
CFLAGS += -I$(INCLUDE)

# 컴파일 옵션을 지정. 라이브러리 관련 gcc옵션은 이곳에 추가하여야 함
hw: $(OBJS)
	$(CC) -o $(BIN)homework $(OBJS) $(LIBNAME) -L$(LIB)

# make clean시 수행할 명령어 (여기서는 오브젝트 파일 지우기)
clean:
	rm -rf $(OBJS)
	
allclean:
	rm -rf $(OBJS)$(BINS)*
