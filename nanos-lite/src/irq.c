#include <common.h>

static Context* do_event(Event e, Context* c) {
  if (e.event == EVENT_YIELD ) {
    printf("EVENT_YIELD, event ID %d\n", e.event);
  }
  else if (e.event == EVENT_SYSCALL ) {
    printf("EVENT_SYSCALL, do_event ID %d, count %d", e.event, c->GPR4); 
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
