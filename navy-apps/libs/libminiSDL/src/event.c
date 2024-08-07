#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *event) {
  unsigned buf_size = 32;
  char *buf = (char *)malloc(buf_size * sizeof(char));
  if (NDL_PollEvent(buf, buf_size) == 1) {
      if (strncmp(buf, "kd", 2) == 0) {
          event->key.type = SDL_KEYDOWN;
      } else {
          event->key.type = SDL_KEYUP;
      }

      int flag = 0;
      for (unsigned i = 0; i < sizeof(keyname) / sizeof(keyname[0]); ++i) {
          if (strncmp(buf + 3, keyname[i], strlen(buf) - 4) == 0 && strlen(keyname[i]) == strlen(buf) - 4) {
              flag = 1;
              event->key.keysym.sym = i;
              break;
          }
      }

      assert(flag == 1);

      free(buf);
      return 1;
  } else {
      return 0;
  }
}

int SDL_WaitEvent(SDL_Event *event) {
  unsigned buf_size = 32;
  char *buf = (char *)malloc(buf_size * sizeof(char));

    while (NDL_PollEvent(buf, buf_size) == 0); 

    if (strncmp(buf, "kd", 2) == 0)
        event->key.type = SDL_KEYDOWN;
    else
        event->key.type = SDL_KEYUP;


    int flag = 0;
    for (unsigned i = 0; i < sizeof(keyname) / sizeof(keyname[0]); ++i) {
        if (strncmp(buf + 3, keyname[i], strlen(buf) - 4) == 0 && strlen(keyname[i]) == strlen(buf) - 4) {
            flag = 1;
            event->key.keysym.sym = i;
            break;
        }
    }

    assert(flag == 1);

    free(buf);
    return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

static unsigned char keystate[sizeof(keyname) / sizeof(keyname[0])];

uint8_t* SDL_GetKeyState(int *numkeys) {
  SDL_Event ev;
    if (SDL_PollEvent(&ev) == 1 && ev.key.type == SDL_KEYDOWN) {
        keystate[ev.key.keysym.sym] = 1;
    } else {
        memset(keystate, 0, sizeof(keystate));
    }
    return keystate;
}
