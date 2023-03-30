#ifndef _FTRACER_H_
#define _FTRACER_H_
#include "../../tools/qemu-diff/include/common.h"

void stack_return(paddr_t cur, paddr_t des);

void stack_call(paddr_t cur, paddr_t des);

void print_stack_trace();

#endif