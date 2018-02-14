#include <stdio.h>
#include <math.h>

#define MAX_ELEMENTS 100001
#define HEAP_FULL(n) (n == MAX_ELEMENTS - 1)
#define HEAP_EMPTY(n) (!n)

int min_heap[MAX_ELEMENTS];
int heap_size = 0;

void insert_min_heap(int item, int *n);
int delete_min_heap(int *n);

int main() {
  int q;
  scanf("%d", &q);
  
  int i;
  int cmd;
  for (i = 0; i < q; i++) {
    scanf("%d", &cmd);
    if (cmd == 0) {
      printf("%d\n", delete_min_heap(&heap_size));
    }
    else {
      insert_min_heap(cmd, &heap_size);
    }
  }
  return 0;
}

// 힙의 적절한 위치에 item을 삽입하는 함수
// item에 0은 올 수 없다.
void insert_min_heap(int item, int *n) {
  int i;
  // 힙이 가득 찼을 경우 경고 메세지 출력
  if (HEAP_FULL(*n)) {
    puts("The heap is full");
    return;
  }

  i = ++(*n);

  // item의 적절한 위치를 찾는다.
  // 우선순위: 1. 절대값이 작은 값
  //           2. 절대값이 같다면 음수의 값
  while ((i != 1)
    && (abs(item) <= abs(min_heap[i / 2])))
  {
    // item의 절대값이 더 작다면
    if (abs(item) < abs(min_heap[i / 2])) {
      min_heap[i] = min_heap[i / 2];
      i /= 2;
    }
    // i/2번 노드가 양수이고 item이 음수라면
    else if (item < min_heap[i / 2]) {
      min_heap[i] = min_heap[i / 2];
      i /= 2;
    }
    // item과 i/2번 노드가 동일한 노드라면
    else if (item == min_heap[i / 2]) {
      break;
    }
    else {
      break;
    }
  }
  min_heap[i] = item;
}

// 힙의 가장 우선순위가 높은 원소를 삭제하는 함수
int delete_min_heap(int *n) {
  // 힙이 비었을 경우 0을 반환한다.
  if (HEAP_EMPTY(*n)) {
    return 0;
  }

  int item, temp;

  // 루트 노드
  item = min_heap[1];
  // 마지막 노드
  temp = min_heap[(*n)--];

  int parent = 1, child = 2;

  // 삭제 후 힙의 크기가 비교 대상 자식보다 크거나 같은 경우
  while (child <= *n) {
    if ((child < *n)
      && (abs(min_heap[child]) >= abs(min_heap[child + 1])))
    {
      // 왼쪽 자식과 오른쪽 자식을 비교한다.
      if (abs(min_heap[child]) > abs(min_heap[child + 1])) {
        child++;
      }
      // 절대값이 같고 child + 1가 음수인 경우
      else if (min_heap[child] > min_heap[child + 1]){
        child++;
      }
      // 두 값이 같다면 왼쪽 자식을 선택한다.
    }

    // 가장 마지막 노드 우선순위가 현재 child의 우선순위보다 낮다면 종료
    if (abs(temp) < abs(min_heap[child])
      || ((abs(temp) == abs(min_heap[child])
        && temp < min_heap[child]))) {
      break;
    }

    // 오른쪽, 왼쪽 자식 중 위의 과정에서 선택된 자식을 부모 노드로 선택
    min_heap[parent] = min_heap[child];
    parent = child;
    child *= 2;
  }
  min_heap[parent] = temp;
  // 삭제 된 원소 반환
  return item;
}
