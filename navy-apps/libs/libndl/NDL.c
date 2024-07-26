#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int picture_w = 0, picture_h = 0;
static int picture_x = 0, picture_y = 0;

int Init_screen(){
   int buf_size = 1024; // 缓冲区大小
    char *buf = (char *)malloc(buf_size * sizeof(char));
    assert(buf != NULL); // 确保内存分配成功

    int fd = open("/proc/dispinfo", 0);
    assert(fd >= 0); // 确保文件打开成功

    ssize_t ret = read(fd, buf, buf_size - 1); // 读取数据 (留出1个字节用于'\0')
    assert(ret >= 0); // 确保读取成功
    buf[ret] = '\0'; // 确保缓冲区是以空字符结尾

    assert(close(fd) == 0); // 关闭文件

    // 使用sscanf提取宽度和高度
    if (sscanf(buf, "WIDTH : %d\nHEIGHT : %d", &screen_w, &screen_h) != 2) {
        fprintf(stderr, "Failed to read width and height.\n");
        free(buf);
        return; // 提取失败时返回
    }

    free(buf); // 释放缓冲区
    return 0;
}

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  memset(buf, 0, len);
  int fd = open("/dev/events", 0, 0);
  int ret = read(fd, buf, len);
  close(fd);
  return ret == 0 ? 0 : 1;
  //return 0;
}

void NDL_OpenCanvas(int *w, int *h) {
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

  if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  }
  //记录画布大小
  picture_w = *w;
  picture_h = *h;
  //计算画布放置的位置
  picture_x=(screen_w - picture_w) / 2;
  picture_y=(screen_h - picture_h) / 2;
  
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fd = open("/dev/fb",0,0);
  printf("fd = %d\n",fd);
  for (int i = 0; i < h; ++i) { 
    lseek(fd, ((y + picture_y + i) * screen_w + (x + picture_x)) * 4, SEEK_SET);
    write(fd, pixels + i * w, 4*w);
    //printf("%d\n",i);
  }
  //close(fd);
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
  Init_screen();
  return 0;
}

void NDL_Quit() {
}
