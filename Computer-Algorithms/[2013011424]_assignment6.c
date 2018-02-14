// 2013011424_성예닮

#include <stdio.h>
#include <stdlib.h>

#define WH 0
#define GR 1
#define BL 2

#define TREE 1
#define BACK 2 
#define FORWARD 3
#define CROSS 4

int time = 0;
int numV = 0;

typedef struct _ual {
  int color;
  int cross;
  int pred;

} ALGraph;

typedef struct _edge {
  int toV;
  int type;

  struct _edge *next;
} Edge;

ALGraph SetV[1000] = { NULL };
Edge *adjList[1000] = { NULL };

void DFS_Visit(int fromV) {
  Edge *edge_fromV;
  edge_fromV = adjList[fromV];

  SetV[fromV].color = GR;
  SetV[fromV].cross = ++time;

  while (edge_fromV != NULL) {
    int toV;
    toV = edge_fromV->toV;

    switch (SetV[toV].color) {

    case WH:
      edge_fromV->type = TREE;
      SetV[toV].pred = fromV;
      DFS_Visit(toV);
      break;

    case GR:
      edge_fromV->type = BACK;
      break;

    case BL:
      if (SetV[fromV].cross < SetV[toV].cross) {
        edge_fromV->type = FORWARD;
      }

      else if (SetV[fromV].cross > SetV[toV].cross) {
        edge_fromV->type = CROSS;
      }

      break;

    default:
      break;
    }

    edge_fromV = edge_fromV->next;
  }

  SetV[fromV].color = BL;
  time++;
}

void addEdge(int fromV, int toV) {
  Edge *newEdge;
  Edge *temp;
  temp = adjList[fromV];

  newEdge = (Edge*)malloc(sizeof(Edge));
  newEdge->toV = toV;
  newEdge->next = NULL;

  if (adjList[fromV] == NULL) {
    adjList[fromV] = newEdge;
  }

  else{
    while (temp->next != NULL) {
      temp = temp->next;
    }

    temp->next = newEdge;
  }
}

void DFS() {
  int i;

  for (i = 1; i <= numV; i++) {
    SetV[i].color = WH;
    SetV[i].pred = NULL;
  }

  time = 0;

  for (i = 1; i <= numV; i++) {
    if (SetV[i].color == WH) {
      DFS_Visit(i);
    }
  }
}

int main() {
  int i = 0;
  int fromV, toV;
  Edge *temp;

  scanf("%d", &numV);

  while (scanf("%d %d", &fromV, &toV) != EOF) {
    addEdge(fromV, toV);
  }

  DFS();

  for (i = 1; i <= numV; i++) {
    temp = adjList[i];

    while (temp != NULL) {
      printf("%d %d %d \n", i, temp->toV, temp->type);
      temp = temp->next;
    }
  }

  return 0;
}

