/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char exp[128];
  uint32_t last;
  /* TODO: Add more members if necessary */
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL, *curr = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    strcpy(wp_pool[i].exp, "\0");
    wp_pool[i].last = 0;
  }

  head = NULL;
  free_ = wp_pool;
}
WP* return_(){
  return head;
}
WP* search_(WP *wp){
  WP* temp=head;
  while(temp){
    if(temp->next==wp) break;
    temp=temp->next;
  }
  return temp;
}
WP* new_wp(){
  if(head==NULL){
    head=free_;
    free_=free_->next;
    head->next=NULL;
    curr=head;
  }
  else{
    if(free_==NULL) assert(0);
    else{
      curr->next=free_;
      curr=free_;
      free_=free_->next;
    }
  }
  return curr;
}
void free_wp(WP *wp){
  if(wp==head){
    head=head->next;
    wp->next=free_;
    free_=wp;
  }
  else if(wp==curr){
    wp->next=free_;
    free_=wp;
    curr=search_(wp);
    curr->next=NULL;
  }
  else{
    wp->next=free_;
    free_=wp;
    search_(wp)->next=wp->next;
  }
};
/* TODO: Implement the functionality of watchpoint */
void check_wp(){
  bool success;
  WP *itr = head;
  while(itr != NULL){
    uint32_t res = expr(itr->exp,&success);
    if(!success){
      printf("The expression of watch point %d is invalid!\n",itr->NO);
    }
    else if(res != itr->last){
      printf("Num:%-6d\t Expr:%-20s\t  New Val:%-14u\t  Old Val:%-14u\n",
        itr->NO,
        itr->exp,
        res,
        itr->last);
      itr->last = res;
      nemu_state.state = NEMU_STOP;
    }
    itr=itr->next;
  }
}
