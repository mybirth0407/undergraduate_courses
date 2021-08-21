#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

// 입력받는 한 줄의 최대 길이
#define CMD_LEN 255

// 트리 하나를 구성하는 노드의 구조체
struct node {
  int value;
  struct node *parent;
  struct node *left_child;
  struct node *right_child;
};

struct node* create_tree();
struct node* search(struct node *root, int key);
void insert_node(struct node *root, int key);
void delete_node(struct node *root, int key);
void show_all(struct node *root);
void delete_all(struct node *root);
int get_height(struct node *root, int key);


int main() {
  struct node* tree = create_tree();

  while (TRUE) {
    char cmd[CMD_LEN];
    fgets(cmd, sizeof(cmd), stdin);
    int key = -1;
    char temp;
    sscanf(cmd, "%c %d", &temp, &key);

    if ('i' == cmd[0]) {
      insert_node(tree, key);
    }
    else if ('d' == cmd[0]) {
      delete_node(tree, key);
    }
    else if ('h' == cmd[0]) {
      struct node *node = search(tree, key);
      if (NULL != node) {
        printf("The height of the node (%d) is %d\n",
                 key, get_height(node, key) - 1);
      }
      else {
        puts("Not found");
      }
    }
    else if ('s' == cmd[0]) {
      show_all(tree->right_child);
      puts("");
    }
    else if ('e' == cmd[0]) {
      delete_all(tree);
      break;
    }
    // 잘못된 입력
    else {
      puts("Only allow command 'i', 'd', 'h', 's', 'e' + [number]");
    }
  }

  return 0;
}

/**
 * 값 0을 가진 트리의 가상 루트 노드를 생성한다.
 */
struct node* create_tree() {
  struct node *root = (struct node *) malloc(sizeof(struct node));
  root->parent = NULL;
  root->value = 0;
  root->left_child = NULL;
  root->right_child = NULL;
  return root;
}

/** 
 * 재귀적으로 key에 해당하는 값을 찾는다.
 * key에 해당하는 값을 가진 노드를 찾지 못하면 NULL을 반환한다.
 */
struct node* search(struct node *root, int key) {
  if (NULL == root) {
    return NULL;
  }
  else if (key == root->value) {
    return root;
  }
  else if (key > root->value) {
    return search(root->right_child, key);
  }
  else if (key < root->value) {
    return search(root->left_child, key);
  }
}

/**
 * BST를 유지하며 노드를 삽입한다.
 * 같은 값은 들어오지 않는다고 가정한다.
 */
void insert_node(struct node *root, int key) {
  if (NULL == root) {
    puts("First please create a tree");
    return;
  }

  // 값을 가진 첫 노드 삽입
  if (0 == root->value && NULL == root->parent
      && NULL == root->left_child && NULL == root->right_child)
  {
    struct node *node = (struct node *) malloc(sizeof(struct node));
    node->value = key;
    node->parent = root;
    node->left_child = NULL;
    node->right_child = NULL;

    root->left_child = node;
    root->right_child = node;
    return;
  }

  // 두번째 이후 노드 삽입
  if (key >= root->value) {
    if (NULL == root->right_child) {
      struct node *node = (struct node *) malloc(sizeof(struct node));
      node->value = key;
      node->parent = root;
      node->left_child = NULL;
      node->right_child = NULL;

      root->right_child = node;
    }
    else {
      insert_node(root->right_child, key);
    }
  }
  else if (key < root->value) {
    if (NULL == root->left_child) {
      struct node *node = (struct node *) malloc(sizeof(struct node));
      node->value = key;
      node->parent = root;
      node->left_child = NULL;
      node->right_child = NULL;

      root->left_child = node;
    }
    else {
      insert_node(root->left_child, key);
    }
  }
}

/**
 * key에 해당하는 값을 가진 노드를 삭제한다.
 * 삭제 대상의 자식 노드의 수에 따라 행동이 다르다.
 * 삭제 대상의 자식 노드의 수
 * 0: 단순 삭제를 진행한다, 해당 노드를 free한다.
 * 1: 삭제를 진행하고, 삭제 대상의 대체 노드로 자식 노드를 선택한다.
 * 2: 삭제를 진행하고, 삭제 대상의 대체 노드로
 *    삭제 대상 노드보다 큰 값 중 가장 작은 노드를 선택한다.
 */
void delete_node(struct node *root, int key) {
  struct node *delete_target = search(root, key);
  if (NULL == delete_target) {
    puts("Not found");
    return;
  }

  // 삭제 대상의 자식 노드가 없는 경우
  if (NULL == delete_target->right_child
      && NULL == delete_target->left_child)
  {
    // 삭제 대상이 삭제 대상의 부모로부터 왼쪽 자식이라면
    if (delete_target->value < delete_target->parent->value) {
      delete_target->parent->left_child = NULL;
      free(delete_target);
    }
    // 삭제 대상이 삭제 대상의 부모로부터 오른쪽 자식이라면
    else if (delete_target->value > delete_target->parent->value) {
      delete_target->parent->right_child = NULL;
      free(delete_target);
    }
  }
  // 삭제 대상의 자식 노드가 오른쪽 하나인 경우
  else if (NULL == delete_target->left_child) {
    // 삭제 대상이 루트 노드일 경우
    if (0 == delete_target->parent->value
        && NULL == delete_target->parent->parent) {
      delete_target->parent->left_child = delete_target->right_child;
      delete_target->parent->right_child= delete_target->right_child;
    }
    // 삭제 대상이 삭제 대상의 부모로부터 왼쪽 자식이라면
    else if (delete_target->value < delete_target->parent->value) {
      delete_target->parent->left_child = delete_target->right_child;
    }
    // 삭제 대상이 삭제 대상의 부모로부터 오른쪽 자식이라면
    else if (delete_target->value > delete_target->parent->value) {
      delete_target->parent->right_child = delete_target->right_child;
    }
    delete_target->right_child->parent = delete_target->parent;
    free(delete_target);
  }
  // 삭제 대상의 자식 노드가 왼쪽 하나인 경우
  else if (NULL == delete_target->right_child) {
    // 삭제 대상이 루트 노드일 경우
    if (0 == delete_target->parent->value
        && NULL == delete_target->parent->parent) {
      delete_target->parent->left_child = delete_target->left_child;
      delete_target->parent->right_child= delete_target->left_child;
    }
    // 삭제 대상이 삭제 대상의 부모로부터 왼쪽 자식이라면
    else if (delete_target->value < delete_target->parent->value) {
      delete_target->parent->left_child = delete_target->left_child;
    }
    // 삭제 대상이 삭제 대상의 부모로부터 오른쪽 자식이라면
    else if (delete_target->value > delete_target->parent->value) {
      delete_target->parent->right_child = delete_target->left_child;
    }
    delete_target->left_child->parent = delete_target->parent;
    free(delete_target);
  }
  // 삭제 대상의 자식 노드가 두개인 경우
  else {
    struct node *parent = delete_target->parent;
    struct node *cur = delete_target->right_child;

    // 오른쪽 자식 노드가 대체 노드인 경우
    if (NULL == cur->left_child) {
      parent->right_child = cur;
    }
    else {
      while (NULL != cur->left_child) {
        cur = cur->left_child;
      }
      cur->parent->left_child = cur->right_child;
      // 대체 노드가 오른쪽 자식을 가질 경우
      if (NULL != cur->right_child) {
        cur->right_child->parent = cur->parent;
      }
      cur->right_child = delete_target->right_child;
    }
    cur->left_child = delete_target->left_child;
    cur->parent = parent;
    // 삭제 대상이 루트 노드일 경우
    if (0 == parent->value && NULL == parent->parent) {
      parent->left_child = cur;
      parent->right_child= cur;
      delete_target->left_child->parent = cur;
    }
    else if (delete_target->value < parent->value) {
      parent->left_child = cur;
    }
    else if (delete_target->value > parent->value) {
      parent->right_child = cur;
    }
    free(delete_target);
  }
}

/**
 * 재귀적으로 트리를 출력한다. (== inorder travel)
 */
void show_all(struct node *root) {
  if (NULL != root) {
    show_all(root->left_child);
    printf("%d ", root->value);
    show_all(root->right_child);
  }
}

/**
 * 재귀적으로 트리를 삭제한다.
 * 가상 노드는 삭제되지 않는다.
 */
void delete_all(struct node *root) {
  if (NULL != root) {
    delete_all(root->left_child);
    free(root);
    delete_all(root->right_child);
  }
}

/**
 * key에 해당하는 값을 가진 노드의 height를 출력한다.
 */
int get_height(struct node *root, int key) {
  if (NULL == root) {
    return 0;
  }
  int left_height = get_height(root->left_child, key);
  int right_height = get_height(root->right_child, key);
  return left_height > right_height ? 1 + left_height: 1 + right_height;
}
