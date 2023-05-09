#include <proc.h>
#include <fs.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

extern void naive_uload(PCB *pcb, const char *filename);
extern void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
extern void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  //naive_uload(NULL, "/bin/dummy");
  //naive_uload(NULL, "/bin/hello");
  //naive_uload(NULL, "/bin/file-test");
  //naive_uload(NULL, "/bin/timer-test");
  //naive_uload(NULL, "/bin/event-test");
  //naive_uload(NULL, "/bin/bmp-test");
  //naive_uload(NULL, "/bin/nslider");
  naive_uload(NULL, "/bin/menu");
  //naive_uload(NULL, "/bin/nterm");
  //naive_uload(NULL, "/bin/bird");
  //naive_uload(NULL, "/bin/pal");
}

Context* schedule(Context *prev) {
  return NULL;
}

// int execve(const char *filename, char *const argv[], char *const envp[]){
//   if (fs_open(filename, 0, 0) == -1){
//     return -1;
//   }
//   printf("Loading from %s ...\n", filename);
//   context_uload(&pcb[program_index], filename, argv, envp);
//   switch_boot_pcb();  
  
//   pcb[0].cp->pdir = NULL;
//   //TODO: 这是一种trade-off
//   //set_satp(pcb[1].cp->pdir);
//   printf("PCB[0] pdir: %p cp: %p\n", pcb[0].cp->pdir, pcb[0].cp);

//   yield();
//   return 0;
// }