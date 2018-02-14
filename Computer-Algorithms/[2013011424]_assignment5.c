// 2013011424_성예닮

#include <stdio.h>
#include <stdlib.h>

#define MAX 100000000

int s[200][200];
int m[200][200];

void seq(int i, int j) { // 괄호치기
  if (i == j) {
    printf(" %d ", i);
  }

  else{
    printf(" ( ");

    seq(i, s[i][j]);
    seq(s[i][j] + 1, j);

    printf(" ) ");
  }
}

void mat(int *p, int n) { // 동적분할
  int i, j, k, l;
  int result;

  for (i = 0; i <= n; i++) {
    m[i][i] = 0;
  }

  for (l = 2; l < n + 1; l++) {
    for (i = 1; i < n - l + 2; i++) {

      j = i + l - 1;
      m[i][j] = MAX;

      for (k = i; k < j; k++) {
        result = m[i][k] + m[k + 1][j] + (p[i - 1] * p[k] * p[j]);

        if (result < m[i][j]) {
          m[i][j] = result;
          s[i][j] = k;
        }
      }
    }
  }  
}

int main() {
  int *p, n, i;

  scanf("%d", &n);

  p = (int *) malloc(sizeof(int) * (n + 1));

  for (i = 0; i < n + 1; i++) {
    scanf("%d", &p[i]);
  }

  mat(p, n);
  printf("%d\n", m[1][n]);

  seq(1, n);
  printf("\n");

  return 0;
}
