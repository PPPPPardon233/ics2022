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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
uint32_t rand_expr[10]={0,0,0,0,1,1,1,2,2,2};
uint32_t rand_op;
char expr_[100];
char op_[2];

#define MAX 1000

static void gen_rand_expr() {
  rand_op =rand()%4;
  
  switch(rand_op){
    case 0:op_[0]='+';op_[1]='\0';break;
    case 1:op_[0]='-';op_[1]='\0';break;
    case 2:op_[0]='*';op_[1]='\0';break;
    default:op_[0]='/';op_[1]='\0';break;
  }

  switch(rand_expr[rand()%10]){
    case 0:{
      sprintf(expr_,"%d",rand()%MAX);
      strcat(buf,expr_);
      //strcat(buf,"U");
      break;
    }
    case 1:{
      strcat(buf,"(");
      gen_rand_expr();
      strcat(buf,")");
      break;
    }
    default:{
      gen_rand_expr();
      strcat(buf,op_);
      gen_rand_expr();
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    strcpy(buf," ");
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
