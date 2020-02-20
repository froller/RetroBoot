#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "confirm.h"

int confirm(const char *subj, ...)
{
  char *hint = " (Type \"yes\"): ";
  size_t len = strlen(subj) + strlen(hint);
  char *subject = (char *)malloc(len + 1);
  strcpy(subject, subj);
  strcat(subject, hint);

  va_list argptr;
  va_start(argptr, subj);
  vprintf(subject, argptr);
  va_end(argptr);

  char input[4] = { 0 };
  scanf("%4s", input);
  return !strcmpi(input, "yes");
}
