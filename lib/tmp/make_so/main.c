#include<stdio.h>
#include"f1.h"
#include"f2.h"
int main()
{
//写两个函数判断ab大小最后输出
  int a = 10;
  int b = 20;
  int big,small;
  big = bigger(a,b);
  small = smaller(a,b);
  printf("bigger:%d\n",big);
  printf("smaller:%d\n",small);

  return 0;
}
