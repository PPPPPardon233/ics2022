#include <stdint.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

void see_if_this_is_loaded(){
  return;
}

int main() {
  see_if_this_is_loaded();
  return _syscall_(SYS_yield, 0, 0, 0);
}
