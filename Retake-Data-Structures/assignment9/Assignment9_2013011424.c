#include <stdio.h>

#define INF 1000000000;
#define N 1001

int node[N][N];
int visit[N];
int dist[N];

int init();
int dijkstra(int start, int end, int n);

int main() {
  init();

  int n;
  scanf("%d", &n);
  int m;
  scanf("%d", &m);

  int i;
  for (i = 0; i < m; ++i) {
    int u, v, w;
    scanf("%d %d %d", &u, &v, &w);
    node[u - 1][v - 1] = w;
  }

  int u, v;
  scanf("%d %d", &u, &v);
  printf("%d \n", dijkstra(u - 1, v - 1, n));

  return 0;
}

int init() {
  int i;
  for (i = 0; i < N; ++i) {
    visit[i] = 0;
    dist[i] = INF;

    int j;
    for (j = 0; j < N; ++j) {
      node[i][j] = INF;
    }
  }
}

int dijkstra(int start, int end, int n) {
  dist[start] = 0;
  int t;
  for (t = 0; t < n; ++t) {
    int min = INF;
    int min_v;

    int i;
    for (i = 0; i < n; ++i) {
      if (visit[i] == 0 && min > dist[i]) {
        min = dist[i];
        min_v = i;
      }
    }
    visit[min_v] = 1;

    for (i = 0; i < n; ++i) {
      if (dist[i] > dist[min_v] + node[min_v][i]) {
        dist[i] = dist[min_v] + node[min_v][i];
      }
    }
  }
  return dist[end];
}

/*
5 8
1 2 2
1 3 3
1 4 6
1 5 10
2 4 3
3 4 1
3 5 4
4 5 1
1 5
*/