#include "biosvid.h"
#include <stdlib.h>
#include <string.h>

void print(const char *str, const unsigned char a = 0)
{
  unsigned char page = 0, row, col, attr;
  biospage(page);
  biosgetpos(page, &row, &col);
  biosgetcha(page, &attr, NULL);
  if (a)
    attr = a;
  biosputs(page, row, col, attr, strlen(str), str);
};

/*
void bprintf(const char *fmt, ...)
{
  char stringBuffer[256];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(stringBuffer, fmt, argptr);
  va_end(argptr);
  print(stringBuffer);
}
*/

void main(void)
{
  print("Nothing\n");
}