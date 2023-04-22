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

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fp = open("/dev/events", O_RDONLY);
  return read(fp, buf, sizeof(char) * len);
}

static int canvas_w = 0, canvas_h = 0;
static int offset_w = 0, offset_h = 0;

void NDL_OpenCanvas(int *w, int *h) {
  screen_w = 400;
  screen_h = 300;
  if (*w == 0 && *h == 0){
    canvas_w = screen_w;
    canvas_h = screen_h;
    *w = screen_w;
    *h = screen_h;
  }
  else{
    canvas_w = *w <= screen_w ? *w : screen_w;
    canvas_h = *h <= screen_h ? *h : screen_h;
  }

  printf("canvas_w = %d, canvas_h = %d, offset_w = %d, offset_h = %d\n", canvas_w, canvas_h, offset_w, offset_h);

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
  FILE *fp;
  uint32_t *p = pixels;
  int i, start_pos = 4 * (x * screen_w + y);
  fp = fopen("/dev/fb", "w");
  for (i = 0; i < h; i++){
    fseek(fp, start_pos, SEEK_SET);
    fwrite(p, 4, w, fp);
    p += w;
    start_pos += screen_w * 4;
  }
  fclose(fp);
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
