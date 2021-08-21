// 201301142_성예닮

#include <stdio.h>
#include <stdlib.h>

void SelectionSort(int arr[], int len, int count) {
  int i, j, minIdx, tmp;

  for (i = 0; i<count; i++) {
    minIdx = i;

    for (j = i + 1; j<len; j++) {
      if (arr[j] < arr[minIdx]) {
        minIdx = j;
      }
    }

    if (i != minIdx) {
      tmp = arr[minIdx];
      arr[minIdx] = arr[i];
      arr[i] = tmp;
    }
  }
}

int main() {
  int i;
  int len, count;
  int arr[30000];
  
  scanf("%d %d", &len, &count);

  for (i = 0; i < len; i++) {
    scanf("%d", &arr[i]);
  }

  SelectionSort(arr, len, count);

  for (i = 0; i < len; i++) {
    printf("%d\n", arr[i]);
  }

  return 0;
}
