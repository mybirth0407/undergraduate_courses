// 2013011424_성예닮

#include <stdio.h>

#define NO 0xFFFFFF

int n, heap[100001], size, print[100001];

void insert(int k) {
  heap[++size] = k;

  int p = size;

  while (heap[p / 2] < k) {
    heap[p] = heap[p / 2];
    p /= 2;
  }
  heap[p] = k;
}

void del() {
  int max, l, r, point = 1, k;

  k = heap[1] = heap[size--];

  while (point <= size / 2) {
    l = point * 2;
    r = point * 2 + 1;

    if (r <= size && heap[r] > heap[l]) {
      max = r;
    }

    else {
      max = l;
    }

    if (k < heap[max]) {
      heap[point] = heap[max];
      point = max;
    }

    else
      break;
  }

  heap[point] = k;
}

int root() {
  return heap[1];
}

int main() {
  int a, i, k;
  heap[0] = NO;

  scanf("%d %d", &n, &k);

  for (i = 0; i < n; i++) {  
    scanf("%d", &a);
    insert(a);
  }

  for (i = n - 1; i >= 0; i--) {
    print[i] = root();
    del();
  }

  for (i = n - 1; i >= n - k; i--) {
    printf("%d ", print[i]);
  }

  printf("\n");

  for (i = n - k - 1; i > -1; i--) {
    printf("%d ", print[i]);
  }

  return 0;
}
