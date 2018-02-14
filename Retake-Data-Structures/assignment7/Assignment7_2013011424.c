#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ELEMENT 1000001

#define SWAP(x,y,t) {t = x; x = y; y = t;}
#define HEAP_FULL(n) (n == MAX_ELEMENT - 1)
#define HEAP_EMPTY(n) (!n)

struct student {
  char name[20];
  int korean;
  int english;
  int math;
};

struct student heap[MAX_ELEMENT];
int n = 0;

void insert_heap(char name[20], int korean, int english, int math, int *n);
int delete_max_heap(int *n);
void adjust(struct student list[], int root, int n);
void heap_sort(struct student list[], int n);

int main() {
  int i = 0;
  // 힙의 크기
  scanf("%d", &n);

  for (i = 0; i < n; i++) {
    scanf("%s %d %d %d",
      heap[i + 1].name, &heap[i + 1].korean,
      &heap[i + 1].english, &heap[i + 1].math);
  }
  heap_sort(heap, n);

  for (i = n; i > 0; --i) {
    puts(heap[i].name);
  }

  return 0;
}

void insert_heap(char name[20], int korean, int english, int math, int *n) {
  struct student s;
  strcpy(s.name, name);
  s.korean = korean;
  s.english = english;
  s.math = math;

  int i;

  if (HEAP_FULL(*n)) {
    fprintf(stderr, "The heap is full.\n");
    exit(1);
  }

  i = ++(*n);

  // 우선순위 힙 생성
  while ((i != 1)
    && ((s.korean >= heap[i / 2].korean)
    || (s.english <= heap[i / 2].english)
    || (s.math >= heap[i / 2].math)
    || (strcmp(s.name, heap[i / 2].name) > 0)))
  {
    if (s.korean > heap[i / 2].korean) {
      heap[i] = heap[i / 2];
      i /= 2;
    }
    else if (s.korean == heap[i / 2].korean
      && s.english < heap[i / 2].english)
    {
      heap[i] = heap[i / 2];
      i /= 2;
    }
    else if (s.korean == heap[i / 2].korean
      && s.english == heap[i / 2].english
      && s.math > heap[i / 2].math)
    {
      heap[i] = heap[i / 2];
      i /= 2;
    }
    else if (s.korean == heap[i / 2].korean
      && s.english == heap[i / 2].english
      && s.math == heap[i / 2].math
      && strcmp(s.name, heap[i / 2].name) > 0) {
      heap[i] = heap[i / 2];
      i /= 2;
    }
    else {
      break;
    }
  }
  heap[i] = s;
}

// int delete_max_heap(int *n) {
//   int item, temp;
//   if (HEAP_EMPTY(*n)) {
//     fprintf(stderr, "The heap is empty\n");
//     exit(1);
//   }

//   item = heap[1];
//   temp = heap[(*n)--];
//   int parent = 1;
//   int child = 2;

//   while (child <= *n) {
//     if ((child < *n) && (heap[child] < heap[child + 1])) {
//       child++;
//     }

//     if (temp >= heap[child]) {
//       break;
//     }

//     heap[parent] = heap[child];
//     parent = child;
//     child *= 2;
//   }

//   heap[parent] = temp;
//   return item;
// }

void adjust(struct student list[], int root, int n) {
  int child;
  struct student temp;
  temp = list[root];
  struct student root_key;
  root_key.korean = list[root].korean;
  root_key.english = list[root].english;
  root_key.math = list[root].math;
  strcpy(root_key.name, list[root].name);

  child = 2 * root;

  while (child <= n) {
    // 루트의 좌 우 자식 중 더 우선순위가 높은 것
    if ((child < n)
      && ((list[child].korean < list[child + 1].korean)

      || (list[child].korean == list[child + 1].korean
        && list[child].english > list[child + 1].english)

      || (list[child].korean == list[child + 1].korean
        && list[child].english == list[child + 1].english
        && list[child].math < list[child + 1].math)

      || (list[child].korean == list[child + 1].korean
        && list[child].english == list[child + 1].english
        && list[child].math == list[child + 1].math
        && strcmp(list[child].name, list[child + 1].name) > 0)))
    {
      child++;
    }

    // 부모와 우선순위가 높은 자식간의 우선순위 비교
    if ((root_key.korean > list[child].korean)

      || (root_key.korean == list[child].korean
        && root_key.english < list[child].english)

      || (root_key.korean == list[child].korean
        && root_key.english == list[child].english
        && root_key.math > list[child].math)

      || (root_key.korean == list[child].korean
        && root_key.english == list[child].english
        && root_key.math == list[child].math
        && strcmp(root_key.name, list[child].name) < 0))
    {
      break;
    }
    // 큰 값을 부모로
    else {
      list[child / 2] = list[child];
      child *= 2;
    }
  }
  list[child / 2] = temp;
}

void heap_sort(struct student list[], int n) {
  int i, j;
  struct student temp;

  for (i = n / 2; i > 0; i--) {
    adjust(list, i, n);
  }
  
  for (i = n - 1; i > 0; i--) {
    SWAP(list[1], list[i + 1], temp);
    adjust(list, 1, i);
  }
}
