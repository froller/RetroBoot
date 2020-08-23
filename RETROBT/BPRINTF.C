#include "bprintf.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <malloc.h>
#include "biosvid.h"

void _Cdecl bprinta(const char *str, const unsigned char attr)
{
  unsigned char page = 0, row, col;
  biospage(page);
  biosgetpos(page, &row, &col);
  biosputs(page, row, col, attr, strlen(str), str);
};
void _Cdecl bprint(const char *str)
{
  unsigned char page = 0, row, col, attr;
  biospage(page);
  biosgetpos(page, &row, &col);
  biosgetcha(page, &attr, NULL);
  biosputs(page, row, col, attr, strlen(str), str);
};
int _Cdecl bprintaf(const char *fmt, const unsigned char attr, ...)
{
  va_list argptr;
  int bytes;
  char *stringBuffer;
  va_start(argptr, fmt);
  bytes = vsprintf(NULL, fmt, argptr);
  va_end(argptr);
  stringBuffer = (char *)alloca(bytes + 1);
  va_start(argptr, fmt);
  vsprintf(stringBuffer, fmt, argptr);
  va_end(argptr);
  bprinta(stringBuffer, attr);
  return bytes;
}
int _Cdecl bprintf(const char *fmt, ...)
{
  va_list argptr;
  int bytes;
  char *stringBuffer;
  va_start(argptr, fmt);
  bytes = vsprintf(NULL, fmt, argptr);
  va_end(argptr);
  stringBuffer = (char *)alloca(bytes + 1);
  va_start(argptr, fmt);
  vsprintf(stringBuffer, fmt, argptr);
  va_end(argptr);
  bprint(stringBuffer);
  return bytes;
}
