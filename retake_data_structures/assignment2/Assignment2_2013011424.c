#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define true 1
#define false 0

#define CMD_LEN 255 // 입력받는 한 줄의 최대 길이

struct Node { // 연결 리스트에 사용할 노드
  int value;
  struct Node *next;
};

struct Node* make_empty_list(); 
void insert(int a, int b, struct Node *list);
void delete(int v, struct Node *list);
struct Node* find_prev(int v, struct Node *list);
struct Node* find(int v, struct Node *)list;
void delete_list(struct Node *list);
void show_list(struct Node *list);

int main() {
  char cmd[CMD_LEN];
  struct Node *list;
  /* 빈 리스트를 생성한다. */
  list = make_empty_list();

  char c;
  int a, b;
  int v;

  while (true) {
    /* 명령줄 하나를 표준 입력으로 받는다. */
    fgets(cmd, sizeof(cmd), stdin);
    switch (cmd[0]) {
    /*  */
    case 'i':
      sscanf(cmd, "%c %d %d", &c, &a, &b);
      insert(a, b, list);
      break;

    /* 입력받은 값이 리스트에 있다면 삭제 */
    case 'd':
      sscanf(cmd, "%c %d", &c, &v);
      delete(v, list);
      break;

    /* 입력받은 값이 리스트에 있다면 해당 값과 이전 값을 출력 */
    case 'f':
      sscanf(cmd, "%c %d", &c, &v);
      /* 연결 리스트에 v라는 값이 없을 경우 */
      if (NULL == find(v, list)) {
        printf("Find error. The %d isn't in the list.\n.", v);
      }
      else {
        struct Node *prev_node = find_prev(v, list);
        /* 연결 리스트의 맨 앞에 노드일 경우 */
        if (0 == prev_node->value) {
          printf("The %d is next of The header.\n", v);
        }
        /* 연결 리스트의 중간 혹은 마지막 노드일 경우 */
        else {
          printf("The %d is next of The %d.\n", v, prev_node->value);
        }
      }
      break;

    /* 전체 리스트를 순서대로 출력 */
    case 's':
      show_list(list);
      break;

    /* 리스트가 생성한 동적 메모리들을 해제하고 프로그램을 종료 */
    case 'e':
      delete_list(list);
      return 0;

    /* 지정된 입력 이외의 입력이 들어왔을 경우 */
    default:
      puts("Only allow 'i', 'd', 'f', 's', 'e'");
    }
  }
  return 0;
}

struct Node* make_empty_list() {
  struct Node *list = (struct Node *) malloc(sizeof(struct Node));
  list->value = 0;
  list->next = NULL;
  return list;
}

void insert(int a, int b, struct Node *list) {
  struct Node *node = (struct Node *) malloc(sizeof(struct Node));
  node->value = a;

  if (0 == b) {
    node->next = list->next;
    list->next = node;
  }
  else {
    struct Node *cur = find(b, list);
    if (NULL == cur) {
      printf("Insert error. The %d isn't in the list.\n", b);
      return;
    }
    node->next = cur->next;
    cur->next = node;
  }
}

void delete(int v, struct Node *list) {
  struct Node *prev_node = find_prev(v, list);
  if (NULL == prev_node) {
    printf("Delete error. The %d isn't in the list.\n", v);
    return;
  }
  struct Node *del_node = prev_node->next;
  prev_node->next = prev_node->next->next;
  free(del_node);
}

struct Node* find_prev(int v, struct Node *list) {
  struct Node *cur = list;
  struct Node *prev_cur = cur;
  while (true) {
    if (v == cur->value) {
      return prev_cur;
    }

    if (NULL == cur->next) {
      break;
    }
    prev_cur = cur;
    cur = cur->next;
  }
  return NULL;
}

struct Node* find(int v, struct Node *list) {
  struct Node *cur = list;
  while (true) {
    if (v == cur->value) {
      struct Node *prev = find_prev(v, list);
      return cur;
    }

    if (NULL == cur->next) {
      break;
    }

    cur = cur->next;
  }
  return NULL;
}

void delete_list(struct Node *list) {
  struct Node *cur = list;
  struct Node *del_node = NULL;
  while (true) {
    if (NULL == cur->next) {
      break;
    }
    del_node = cur;
    cur->next = cur->next->next;
  }
}

void show_list(struct Node *list) {
  struct Node *cur = list->next;
  if (NULL == cur) {
    puts("There are no elements in the list.");
    return;
  }

  while (true) {
    printf("%d ", cur->value);
    if (NULL == cur->next) {
      break;
    }
    cur = cur->next;
  }
  puts("");
}

