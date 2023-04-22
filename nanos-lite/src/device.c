#include <common.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *str = (char *)buf;
  for (int i = 0; i < len; ++i){
    putch(str[i]);
  }
  return len;
}

extern void switch_program_index(int new_index);

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) {
    *(char*)buf = '\0';
    return 0;
  }
  int ret = snprintf(buf, len, "%s %s\n", ev.keydown?"k↓":"k↑", keyname[ev.keycode]);
  return ret;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T screen_cfg = io_read(AM_GPU_CONFIG);
	sprintf(buf, "[W]: [%d]\n[H]: [%d]", screen_cfg.width, screen_cfg.height);
	return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  uintptr_t *ptr;
  ptr = (uintptr_t *)(&buf);

  io_write(AM_GPU_MEMCPY, offset, (void *)*ptr, len);
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
