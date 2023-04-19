#include <common.h>

static Context* do_event(Event e, Context* c) {
  // switch (e.event) {
  //   default: panic("Unhandled event ID = %d", e.event);
  // }

  // return c;
  if (e.event == EVENT_YIELD ) {
    printf("EVENT_YIELD, event ID %d\n", e.event);

  } else {
    panic("PANIC event ID %d", e.event);
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
