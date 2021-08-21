// 2013011424_성예닮

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WH 0
#define GR 1
#define BL 2

#define TREE 1
#define BACK 2 
#define FORWARD 3
#define CROSS 4

typedef struct _ual {
  int color;
  int f;
  int d;
} ALGraph;

typedef struct _GraphInfo {
  int out_degree;
  int *list;
} GraphInfo;

ALGraph *vertex = NULL;
GraphInfo *vertexInfo = NULL;

int numV = 0;
int time = 0;
int *topoList = NULL;
int topo_num = 0;
int isDAG = 1;

void addEdge(int fromV, int toV) {
  int i = 0;
  int cur = 0;

  for (i = 0; i < numV; i++) {
    if (vertexInfo[fromV].list[i] == 0) {
      break;
    }

    cur++;
  }

  vertexInfo[fromV].list[cur] = toV;
}

void DFS_Visit(int fromV) {
  int i = 0;
  int toV = 0;

  time++;

  vertex[fromV].color = GR;
  vertex[fromV].d = time;

  for (i = 0; i < vertexInfo[fromV].out_degree; i++) {
    toV = vertexInfo[fromV].list[i];

    switch (vertex[toV].color) {

    case WH:

      DFS_Visit(toV);
      break;

    case GR:

      isDAG = 0;
      break;

    case BL:
    default:

      break;
    }
  }

  vertex[fromV].color = BL;
  topoList[numV - (topo_num++)] = fromV;

  time++;
  vertex[fromV].f = time;
}

void Sort(int *list, int num) {
  int i = 0, j = 0, tmp = 0;

  for (i = 0; i < num; i++) {
    for (j = i + 1; j < num; j++) {
      if (list[i] > list[j]) {
        tmp = list[i];
        list[i] = list[j];
        list[j] = tmp;
      }
    }
  }
}

void DFS() {
  int i = 0;

  for (i = 1; i <= numV; i++) {
    vertex[i].d = 0;
    vertex[i].f = 0;
    vertex[i].color = WH;

    Sort(vertexInfo[i].list, vertexInfo[i].out_degree);
  }

  time = 0;

  for (i = 1; i <= numV; i++) {
    if (vertex[i].color == WH) {
      DFS_Visit(i);
    }
  }
}

int main() {
  int i = 0, j = 0;
  int fromV, toV;

  scanf("%d", &numV);

  vertexInfo = (GraphInfo*)malloc(sizeof(GraphInfo)*(numV + 1));

  for (i = 1; i <= numV; i++) {
    vertexInfo[i].list = (int*)malloc(sizeof(int)*numV);
    vertexInfo[i].out_degree = 0;
  }

  for (i = 1; i <= numV; i++) {
    for (j = 0; j < numV; j++) {
      vertexInfo[i].list[j] = 0;
    }
  }

  vertex = (ALGraph *) malloc(sizeof(ALGraph) * (numV + 1));
  topoList = (int *) malloc(sizeof(int) * (numV + 1));

  while (scanf("%d %d", &fromV, &toV) != EOF) {
    addEdge(fromV, toV);
    vertexInfo[fromV].out_degree++;
  }

  DFS();

  printf("%d\n", isDAG);

  if (isDAG) {
    for (i = 1; i <= numV; i++) {
      printf("%d ", topoList[i]);
    }
  }

  return 0;
}
