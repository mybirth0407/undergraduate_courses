// 2013011424_성예닮

#include <stdio.h>
#include <stdlib.h>

void Merge(int arr[], int left, int mid, int right) {
  int fIdx = left;
  int rIdx = mid + 1;
  int i;

  int *sortArr = (int*)malloc(sizeof(int)*(right + 1));
  int sIdx = left;

  while (fIdx <= mid && rIdx <= right) {
    if (arr[fIdx] <= arr[rIdx]) {
      sortArr[sIdx] = arr[fIdx++];
    }

    else{
      sortArr[sIdx] = arr[rIdx++];
    }
    sIdx++;
  }

  if (fIdx > mid) {
    for (i = rIdx; i <= right; i++, sIdx++) {
      sortArr[sIdx] = arr[i];
    }
  }

  else{
    for (i = fIdx; i <= mid; i++, sIdx++) {
      sortArr[sIdx] = arr[i];
    }
  }

  for (i = left; i <= right; i++) {
    arr[i] = sortArr[i];
  }

  free(sortArr);
}
  

void MergeSort(int arr[], int left, int right) {
  int mid;

  if (left < right) {
    mid = (left + right) / 2;
    MergeSort(arr, left, mid);
    MergeSort(arr, mid + 1, right);
    Merge(arr, left, mid, right);
  }
}

int main() {
  int arr[100000];
  int i;
  int n;

  scanf("%d", &n);

  for (i = 0; i < n; i++) {
    scanf("%d", &arr[i]);
  }

  MergeSort(arr, 0, n-1);

  for (i = n - 1; i > -1; i--) {
    printf("%d\n", arr[i]);
  }

  return 0;
}
