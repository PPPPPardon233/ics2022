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

static AM_GPU_CONFIG_T gpu_config;
static AM_GPU_FBDRAW_T gpu_fbdraw;

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

static char *__itoa(int num, char *buff){
  char tmp[32];
  if (num == 0){
    strcat(buff, "0");
    return buff;
  }

  uint8_t i = 0;
  while (num != 0){
    tmp[i] = num % 10 + '0';
    num /= 10;
    i++;
  }

  for (int j = i - 1; j >= 0; --j)
    buff[i - 1 - j] = tmp[j];
  buff[i] = '\0';

  return buff;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  int width = gpu_config.width, height = gpu_config.height;

  char num_buf[32];

  strcpy(buf, "WIDTH:");
  strcat(buf, __itoa(width, num_buf));
  strcat(buf, "\nHEIGHT:");
  strcat(buf, __itoa(height, num_buf));
  strcat(buf, "\n");
  return strlen((char *)buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  if (len == 0){
    // printf("end!\n");
    gpu_fbdraw.sync = 1;
    gpu_fbdraw.w = 0;
    gpu_fbdraw.h = 0;
    ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
    return 0;
  }

  int width = gpu_config.width;
  // printf("screen is %d\n", width);

  gpu_fbdraw.pixels = (void *)buf;
  gpu_fbdraw.w = len;
  gpu_fbdraw.h = 1;
  gpu_fbdraw.x = offset % width;
  gpu_fbdraw.y = offset / width;
  // printf("w,h is %d,%d\n",gpu_fbdraw.w,gpu_fbdraw.h);
  gpu_fbdraw.sync = 0;
  ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
