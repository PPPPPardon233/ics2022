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
  if (offset > 0){
    return 0;
  }

  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;

  int ret = snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d", w, h);
  //Log("%s", (char *)buf);
  if (ret >= len){
    assert(0);
  }
  return ret + 1;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int32_t w = io_read(AM_GPU_CONFIG).width;
	uint32_t h = io_read(AM_GPU_CONFIG).height;
  uint32_t x = (offset/4)%w;//x
  uint32_t y = (offset/4)/w;//y
  if(offset+len > w*h*4) len = w*h*4 - offset;
  io_write(AM_GPU_FBDRAW, x, y, (uint32_t*)buf, len/4, 1, true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
