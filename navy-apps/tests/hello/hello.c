#include <unistd.h>
#include <stdio.h>

int main() {
  write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  while (j!=100000) {
    printf("Hello World from Navy-apps for the %dth time!\n", i ++);
    j++;
  }
  return 0;
}
