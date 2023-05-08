#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

typedef struct size
{
  int w;
  int h;
} Size;
Size disp_size;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fp = open("/dev/events", O_RDONLY);
  return read(fp, buf, sizeof(char) * len);
}

static int canvas_w, canvas_h, canvas_x = 0, canvas_y = 0;

void NDL_OpenCanvas(int *w, int *h) {

  if (*w == 0 && *h == 0){
    *w = disp_size.w;
    *h = disp_size.h;
  }

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  if (w == 0 && h == 0){
    w = disp_size.w;
    h = disp_size.h;
  }
  assert(w > 0 && w <= disp_size.w);
  assert(h > 0 && h <= disp_size.h);

  // write(1, "here\n", 10);
  // printf("draw [%d, %d] to [%d, %d]\n", w, h, x, y);
  for (size_t row = 0; row < h; ++row){
    // printf("draw row %d with len %d\n", row, w);
    lseek(fbdev, x + (y + row) * disp_size.w, SEEK_SET);
    // printf("pixels pos %p with len %d\n",pixels + row * w, w);
    write(fbdev, pixels + row * w, w);
    // printf("draw row %d with len %d\n", row, w);
  }
  write(fbdev, 0, 0);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}


int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
