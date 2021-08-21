// 2013011424_성예닮

#include <stdio.h>

int main(){
  int arr[30000];
  int n = 0, temp = 0;
  int i = 0, j = 0, k = 0;

  scanf("%d", &n);

  for (i = 0; i < n; i++){
    scanf("%d", &arr[i]);
  }

  for (i = 1; i < n; i++){
    temp = arr[i];

    for (j = 0; j < i; j++){
      if (arr[j] > temp){
        for (k = i; k > j; k--){
          arr[k] = arr[k - 1];
        }

        arr[j] = temp;
        break;
      }
    }
  }

  printf("\n");

  for (i = n - 1; i > -1; i--){
    printf("%d\n", arr[i]);
  }

  return 0;
}
