#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define CMD_LEN 255
#define MAX_ARG 5

typedef struct employee {
  char name[10];
  int age;
  int salary;
  char department[20];
} Employee;

void insert(Employee **, Employee *, int);
void delete(Employee **, char *, int);
int find(Employee **, char *, int);
void inform(Employee **, char *, int);
int avgBenefit(Employee **, char *, int);

int main() {
  char command[CMD_LEN];  // 명령어 길이 제한 255
  char *parse[MAX_ARG];  // 명령어 인자 수 제한 5

  int n;  // 공간의 크기
  int count;  

  Employee **employees = NULL;

  while (1) {
    count = 1;
    /* 한 줄을 문자열로 입력받는다. */
    fgets(command, sizeof(command), stdin);
    int i;

    /* 명령어의 토큰 개수를 구한다. */
    for (i = 0; i < strlen(command); i++) {
      if (command[i] == ' ') {
        count++;
      }
    }

    /* 입력받은 문자열이 개행 문자라면 프로그램을 종료한다. */
    if (command[0] == '\n') {
      /* 저장된 사원이 있다면 모두 메모리를 해제한다. */
      if (employees != NULL) {
        for (i = 0; i < n; i++) {
          free(employees[i]);
        }
        free(employees);
      }
      puts("Enter key to end the program.");
      break;
    }

    /* 입력받은 문자열을 파싱하여 parse 배열에 저장한다. */
    parse[0] = strtok(command, " ");
    for (i = 1; i < count; i++) {
      parse[i] = strtok(NULL, " ");
    }
    /* 마지막 파싱 문자열의 개행 문자는 삭제한다. */
    parse[count - 1] = strtok(parse[count - 1], "\n");

    /* 각 명령어와 문자열을 비교해서, 해당하는 명령을 실행한다. */
    if (!strcmp(parse[0], "set")) {
      /* atoi 함수를 이용하여 문자열을 숫자로 변환한다. */
      n = atoi(parse[1]);
      employees = (Employee **) malloc(sizeof(Employee *) * n);
      int i;
      for (i = 0; i < n; i++) {
        employees[i] = NULL;
      }
    }
    else if (!strcmp(parse[0], "insert")) {
      Employee *employee = (Employee *) malloc(sizeof(Employee));
      /* 문자열 복사 */
      strcpy(employee->name, parse[1]);
      strcpy(employee->department, parse[4]);
      employee->age = atoi(parse[2]);
      employee->salary = atoi(parse[3]);
      insert(employees, employee, n);
    }
    else if (!strcmp(parse[0], "delete")) {
      char name[10];
      strcpy(name, parse[1]);
      delete(employees, name, n);
    }
    else if (!strcmp(parse[0], "find")) {
      char name[10];
      strcpy(name, parse[1]);
      /* 사원이 존재한다면 위치를 반환, 아니라면 -1을 반환한다. */
      printf("%d\n", find(employees, name, n));
    }
    else if (!strcmp(parse[0], "inform")) {
      char name[10];
      strcpy(name, parse[1]);
      inform(employees, name, n);
    }
    else if (!strcmp(parse[0], "avg")) {
      char department[20];
      strcpy(department, parse[1]);
      int x = avgBenefit(employees, department, n);
      /* 부서가 존재한다면 평균을, 존재하지 않는 부서라면 0을 반환한다. */
      printf("%d\n", x);
    }
    /* 지정된 명령어가 아닌 다른 명령어를 입력했을 경우 출력한다. */
    else {
      puts("Usage:");
      puts("\tinsert [name], [age], [salary], [department]");
      puts("\tdelete [name]");
      puts("\tfind [name]");
      puts("\tinform [name]");
      puts("\tavg [department]");
    }
  }
  return 0;
}

void insert(Employee **es, Employee *e, int n) {
  /* 아직 할당되지 않은 공간이라면 경고 메세지를 출력한다. */
  if (es == NULL) {
    puts("First, enter the set command");
    return;
  }

  int i;
  for (i = 0; i < n; i++) {
    /* 처음으로 발견된 빈 공간에 삽입한다. */
    if (es[i] == NULL) {
      es[i] = e;
      return;
    }
  }
  puts("Not enough space");
}

void delete(Employee **es, char *name, int n) {
  if (es == NULL) {
    puts("First, enter the set command");
    return;
  }

  int i;
  for (i = 0; i < n; i++) {
    /* 빈 공간이 아니고, 이름이 일치한다면 */
    if (es[i] != NULL && !strcmp(es[i]->name, name)) {
      /* 메모리를 해제한다. */
      free(es[i]);
      es[i] = NULL;
      return;
    }
  }
  puts("Not found");
}

int find(Employee **es, char *name, int n) {
  if (es == NULL) {
    puts("First, enter the set command");
    return -1;
  }

  int i;
  for (i = 0; i < n; i++) {
    if (es[i] != NULL && !strcmp(es[i]->name, name)) {
      return i;
    }
  }
  return -1;
}

void inform(Employee **es, char *name, int n) {
  if (es == NULL) {
    puts("First, enter the set command");
    return;
  }

  int success = 0;  // 일치하는 사원의 개수

  int i;
  for (i = 0; i < n; i++) {
    if (es[i] != NULL && !strcmp(es[i]->name, name)) {
      printf("%s %d %d %s\n",
             es[i]->name, es[i]->age, es[i]->salary, es[i]->department);
      success++;
    }
  }

  /* 일치하는 사원이 없다면 */
  if (!success) {
    puts("Not found");
  }
}

int avgBenefit(Employee **es, char *department, int n) {
  if (es == NULL) {
    puts("First, enter the set command");
    return -1;
  }

  /* 월급의 합 */
  int sum = 0;
  /* 해당 부서의 사원의 수 */
  int nDepartments = 0;

  int i;
  for (i = 0; i < n; i++) {
    if (es[i] != NULL && !strcmp(es[i]->department, department)) {
      sum += es[i]->salary;
      nDepartments++;
    }
  }

  /* 없는 부서라면 0을 반환한다. */
  if (sum == 0) {
    return 0;
  }
  return (sum / nDepartments);
}

