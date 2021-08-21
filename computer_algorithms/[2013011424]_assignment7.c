// 2013011424_성예닮

#include <stdio.h>
#include <stdlib.h>

int numV = 0;
int *list = NULL;
int *Qlist = NULL;
int *rep = NULL;

typedef struct _vertex {
  int groupname;
  int value;
  int rank;

  struct _vertex *parent;
} Vertex;

Vertex *vertex;

void Makeset(Vertex *x, int value) {
  x->value = value;
  x->parent = x;
  x->rank = 0;
}

Vertex *Find(Vertex *x) {
  if (x != x->parent) {
    x->parent = Find(x->parent);
  }

  return x->parent;
}

void Link(Vertex *x, Vertex *y) {
  if (x->rank > y->rank) {
    y->parent = x;      //랭크가 높은 놈이 부모가 되도록
  }
  else{
    x->parent = y;
    if (x->rank == y->rank) {
      y->rank = ++(y->rank);
    }
  }
}

void Union(Vertex *x, Vertex *y) {

  Link(Find(x), Find(y));
}

int Samexy(Vertex *x, Vertex *y) {

  if (Find(x) == Find(y)) {

    return 1;
  }

  else{

    return 0;
  }
}

int main() {

  int *tmp_list = NULL;
  int v1 = 0;
  int v2 = 0;
  int i = 0, j = 0, tmp = 0;
  int order = 0;
  int compNUM = 0;

  scanf("%d", &numV);

  vertex = (Vertex*)malloc(sizeof(Vertex)*(numV + 1));
  list = (int*)malloc(sizeof(int)*(numV + 1));
  tmp_list = (int*)malloc(sizeof(int)*(numV + 1));

  for (i = 1; i <= numV; i++) {

    Makeset(&vertex[i], i);
  }

  while (scanf("%d %d", &v1, &v2) != EOF) {

    if (Find(&vertex[v1]) != Find(&vertex[v2])) {

      Union(&vertex[v1], &vertex[v2]);
    }
  }

  for (i = 1; i <= numV; i++) {

    list[i] = Find(&vertex[i])->value;
    tmp_list[i] = Find(&vertex[i])->value;
  }

  for (i = 1; i <= numV; i++) {

    tmp = list[i];

    if (tmp != 0) {

      compNUM++;

      for (j = i + 1; j <= numV; j++) {

        if (tmp == list[j]) {

          list[j] = 0;
        }
      }
    }
  }

  Qlist = (int*)malloc(sizeof(int)*(compNUM + 1));
  order = 1;

  for (i = 1; i <= numV; i++) {

    if (list[i] != 0) {

      Qlist[order++] = list[i];
    }
  }

  for (i = 1; i <= numV; i++) {

    for (j = 1; j <= numV; j++) {

      if (tmp_list[i] == Qlist[j]) {

        list[i] = j;
      }
    }
  }

  printf("%d\n", compNUM);

  for (i = 1; i <= numV; i++) {

    printf("%d\n", list[i]);
  }

  return 0;
}
