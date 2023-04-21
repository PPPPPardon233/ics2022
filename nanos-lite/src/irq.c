#include <common.h>

extern void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  if (e.event == EVENT_YIELD ) {
    printf("EVENT_YIELD, event ID %d\n", e.event);
    do_syscall(c);
  }
  else if (e.event == EVENT_SYSCALL ) {
    printf("EVENT_SYSCALL, do_event ID %d, count %d\n", e.event, c->GPR4); 
    do_syscall(c);
  }
  else {
    panic("PANIC event ID %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
