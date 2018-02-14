 #include <stdio.h>

#define MAX 1001

#define SWAP(x,y,t) {t=x; x=y; y=t;}

void quicksort(int list[], int left, int right);

int main() {
  int n;
  scanf("%d", &n);
  int persons[MAX];
  int i;

  for (i = 0; i < n; ++i) {
    scanf("%d", &persons[i]);
  }

  quicksort(persons, 0, n);

  int rank = 1;
  int temp = 0;

  for (i = n; i > 0; --i) {
    if (persons[i] == persons[i - 1]) {
      printf("%d %d\n", persons[i], rank);
      temp++;
    }
    else {
      printf("%d %d\n", persons[i], rank++);
      rank += temp;
      temp = 0;
    }
  }
  return 0;
}


void quicksort(int list[], int left, int right) {
  int pivot, i, j;
  int temp;

  if (left < right) {
    i = left; j = right + 1;
    pivot = list[left];

    do  {
      do {
        i++;
      } while (list[i] < pivot);

      do {
        j--;
      } while (list[j] > pivot);

      if (i < j) {
        SWAP(list[i], list[j], temp);
      }
    } while (i < j);

    SWAP(list[left], list[j], temp);
    // j는 정렬된 위치
    quicksort(list, left, j - 1);
    quicksort(list, j + 1, right);
  }
}
