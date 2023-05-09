#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

struct __event_element{
  uint8_t type;
  uint8_t sym;
  struct __event_element* next;
};
typedef struct __event_element event_element;

static event_element event_queue = {.type = 0, .sym = 0, .next = NULL};
static event_element *end = &event_queue;

static void append(uint8_t type, uint8_t sym){
  event_element *new_element = malloc(sizeof(event_element));
  new_element->type = type;
  new_element->sym = sym;
  new_element->next = NULL;
  end->next = new_element;
  end = new_element;
}

static int pop(uint8_t *type, uint8_t *sym){
  if (event_queue.next == NULL){
    return 0;
  }else {
    event_element *buf = event_queue.next;
    *type = buf->type;
    *sym = buf->sym;
    event_queue.next = buf->next;
    if (buf == end){
      end = &event_queue;
    }
    free (buf);
  }
  return 1;
}

static uint8_t key_state[sizeof(keyname) / sizeof(keyname[0])] = {0};


int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

static char key_buf[64], *key_action, *key_key;

#define _DEBUG_

static int inline read_keyinfo(uint8_t *type, uint8_t *sym){
  int ret = NDL_PollEvent(key_buf, sizeof(key_buf));
  if (!ret){
    return 0;
  }

  #ifdef _DEBUG_
  printf("%s\n", key_buf);
  #endif
  //deal with key_action
  key_action = key_buf;
  int i;
  for (i = 0; key_buf[i] != ' '; i++){}
  key_buf[i] = '\0';

  //deal with key_key
  key_key = &key_buf[i + 1]; 

  #ifdef _DEBUG_
  printf("%s",key_key);
  #endif

  for (i = 0;  key_key[i] != '\0' && key_key[i] != '\n'; i++){}
  if (key_key[i] == '\n'){
    key_key[i] = '\0';
  }

  //deal with paramaters
  if (key_action[1] == '↓')   *type = SDL_KEYDOWN;
  else                        *type = SDL_KEYUP;

  for (i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
    if (key_key[0] == keyname[i][0] && strcmp(key_key, keyname[i]) == 0){
      *sym = i;
    }
  }
  return 1;
}

int SDL_PollEvent(SDL_Event *ev) {
  uint8_t type = 0, sym = 0;
  if (read_keyinfo(&type, &sym)){
    ev->type = type;
    ev->key.keysym.sym = sym;

    switch(type){
    case SDL_KEYDOWN:
      key_state[sym] = 1;
      #ifdef _DEBUG_
      printf("%d Down\n", (int)sym);
      #endif
      break;
    
    case SDL_KEYUP:
      key_state[sym] = 0;
      #ifdef _DEBUG_
      printf("%d Up\n", (int)sym);
      #endif
      break;
    }
  }
  else {
    return 0;
  }
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  uint8_t type = 0, sym = 0;
  while (!read_keyinfo(&type, &sym)){}
  
  event->type = type;
  event->key.keysym.sym = sym;

  switch(type){
    case SDL_KEYDOWN:
      key_state[sym] = 1;
      break;
    
    case SDL_KEYUP:
      key_state[sym] = 0;
      break;
  }
  #ifdef _DEBUG_
  printf("catch event\n");
  #endif
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  SDL_Event ev;

  if (numkeys)
    *numkeys = sizeof(key_state) / sizeof(key_state[0]);
  //SDL_PumpEvents();
  return key_state;
}