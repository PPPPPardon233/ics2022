#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <proc.h>
#include <sys/time.h>

/*
*To make it easier to code,here are all the SYS_interupt types
enum {
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};
*/

int sys_yield(){
	yield();
	return 0;
}

int sys_exit(){
	halt(0);
	return 0;
}

int sys_gettimeofday(Context *c)
{
  struct timeval *tv = (struct timeval *)c->GPR2;
  __uint64_t time = io_read(AM_TIMER_UPTIME).us;
  tv->tv_usec = (time % 1000000);
  tv->tv_sec = (time / 1000000);
  c->GPRx = 0;
	return 0;
}

void sys_write(Context *c){
	// if (c->GPR2 == 1 || c->GPR2 == 2){
  //   for (int i = 0; i < c->GPR4; ++i){
  //     putch(*(((char *)c->GPR3) + i));
  //   }
  //   c->GPRx = c->GPR4;
  // }
  // else  
    c->GPRx = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
}

int sys_open(Context *c) {
  int ret = fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
  return ret;
}

int sys_read(Context *c){
  return fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
}

int sys_close(Context *c){
  return fs_close(c->GPR2);
}

int sys_lseek(Context *c){
  return fs_lseek(c->GPR2, c->GPR3, c->GPR4);
}

int sys_brk(Context *c){
  return 0;
}

void do_syscall(Context *c) {
  Log("strace", c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);

	uintptr_t a[4];
	a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  switch(a[0]){
    case SYS_exit:
      c->GPRx = sys_exit();
      break;
    case SYS_yield:
      c->GPRx = sys_yield();
      break;
    case SYS_write:
      sys_write(c);
      break;
    case SYS_brk:
      c->GPRx = sys_brk(c);
      break;
    case SYS_open:
      sys_open(c);
      break;
    case SYS_close:
      sys_close(c);
      break;
    case SYS_read:
      sys_read(c);
      break;
    case SYS_lseek:
      sys_lseek(c);
      break;
    case SYS_gettimeofday:
      sys_gettimeofday(c);
      break;
    default:
      panic("Unhandled syscall ID = %d", a[0]);
  }
}
