#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {} WP;
WP* new_wp();
void free_wp(int des);

#endif