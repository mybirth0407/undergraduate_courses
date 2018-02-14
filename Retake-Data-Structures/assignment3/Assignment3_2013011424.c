#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>


#define true 1
#define false 0

#define CMD_LEN 255 // 입력받는 한 줄의 최대 길이
#define STACK_SIZE 255 // 스택 하나의 크기
#define STACK_CNT 255 // 스택의 개수

struct Stack {
  char *array;
  int size; // 스택의 최대 원소 개수
  int top; // 스택의 원소 개수 - 1
};

struct Stack* createStack(int stackCnt, int stackSize);
void makeEmptyStack(struct Stack *s);
int isEmpty(struct Stack *s);
int isFull(struct Stack *s);
void push(char x, struct Stack *s);
char* pop(struct Stack *s);
void deleteStack(struct Stack *s);

int main() {
  int i;
  while (true) {
    // 스택을 넉넉히 초기화
    struct Stack *s = createStack(STACK_CNT, STACK_SIZE);
    char *output = (char *) malloc(sizeof(char) * CMD_LEN);

    // output index
    int k = 0;
    int openCnt = 0;
    int closeCnt = 0;
    int ignore = false;

    char cmd[CMD_LEN];
    fgets(cmd, sizeof(cmd), stdin);
    
    // 프로그램 종료
    if ('!' == cmd[0]) {
      for (i = 0; i < STACK_SIZE; i++) {
        deleteStack(&s[i]);
      }
      free(s);
      break;
    }

    for (i = 0; i < strlen(cmd); i++) {
      // 여는 괄호
      if ('(' == cmd[i]) {
        openCnt++;
      }
      // 닫는 괄호
      else if (')' == cmd[i]) {
        int j;
        // 스택이 비어있을 때 ')'가 입력되면 뒤의 문자들은 무시
        // 스택의 개수가 입력받은 명령줄의 길이보다 많을 수 없음
        for (j = 0; j < strlen(cmd); j++) {
          if (!isEmpty(&s[j])) {
            break;
          }
        }
        if (j == strlen(cmd)) {
          ignore = true;
        }

        if (openCnt - closeCnt - 1 >= 0) {
          while (true) {
            if (isEmpty(&s[openCnt - closeCnt - 1])) {
              break;
            }
            output[k++] = *(pop(&s[openCnt - closeCnt - 1]));
          }
        }
        else {

        }
        closeCnt++;
      }
      // 알파벳
      else if ('a' <= cmd[i] && 'z' >= cmd[i]) {
        // 괄호가 열려있다면
        if (openCnt > closeCnt) {
          push(cmd[i], &s[openCnt - closeCnt - 1]);
        }
        // 닫힌 괄호 뒤의 문자
        else if (closeCnt > openCnt) {

        }
        // 스택이 비어있을 때 알파벳 입력
        else {
          if (!ignore) {
            output[k++] = cmd[i];
          }
        }
      }
      // 출력 문자
      else if ('#' == cmd[i]) {
        if (openCnt == closeCnt) {
          printf("right. ");
        }
        else {
          printf("wrong. ");
        }
        
        int j;
        for (j = 0; j < k; j++) {
          printf("%c ", output[j]);
        }
        puts("");
        free(output);
      }
    }
  }
  return 0;
}

struct Stack* createStack(int stackCnt, int stackSize) {
  struct Stack *s = (struct Stack *) malloc(sizeof(struct Stack) * stackCnt);
  int i;

  for (i = 0; i < stackSize; i++) {
    s[i].size = stackSize;
    s[i].array = (char *) malloc(sizeof(char) * stackSize);
    s[i].top = -1;
  }

  return s;
}

void makeEmptyStack(struct Stack *s) {
  s->top = -1;
}

int isEmpty(struct Stack *s) {
  if (s->top == -1) {
    return true;
  }
  return false;
}

int isFull(struct Stack *s) {
  if (s->size - 1 == s->top) {
    return true;
  }
  return false;
}

void push(char x, struct Stack *s) {
  if (isFull(s)) {
    puts("스택이 가득 찼습니다.");
    return;
  }

  s->array[s->top + 1] = x;
  s->top++;
}

char* pop(struct Stack *s) {
  if (isEmpty(s)) {
    puts("스택이 비어 있습니다.");
    return NULL;
  }

  char *x = &s->array[s->top];
  s->top--;
  return x;
}

void deleteStack(struct Stack *s) {
  free(s->array);
}
