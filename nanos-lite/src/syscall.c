#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <proc.h>
#include <sys/time.h>

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

int sys_yield()
{
	yield();
	return 0;
}

int sys_exit()
{
	halt(0);
	return 0;
}

int sys_write(int fd, const void *buf, size_t count)
{
	return fs_write(fd, buf, count);
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

void do_syscall(Context *c) {
  Log("strace", c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);

	uintptr_t a[4];
	a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  if ( a[0] == SYS_exit) {
    c->GPRx = sys_exit();
  } else if ( a[0] == SYS_yield) {
    c->GPRx = sys_yield();
  } else if ( a[0] == SYS_write )  {
    c->GPRx = sys_write(a[1], (void *)a[2], a[3]);
  } else if ( a[0] == SYS_open ) {
    c->GPRx = sys_open(c);
  } else if ( a[0] == SYS_read) {
    c->GPRx = sys_read(c);
  } else if ( a[0] == SYS_close) {
    c->GPRx = sys_close(c);
  } else if ( a[0] == SYS_lseek) {
    c->GPRx = sys_lseek(c);
  } else if ( a[0] == SYS_gettimeofday) {
    c->GPRx = sys_gettimeofday(c);
  } else if ( a[0] == SYS_brk) {
    c->GPRx = 0;
  } else {
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
