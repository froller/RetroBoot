#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include "error.h"

int diskerror = 0;

char const *diskerrormessage[] = {
/* 0x00 */ "No error",
/* 0x01 */ "Bad command",
/* 0x02 */ "Address mark not found",
/* 0x03 */ "Attempt to write to write-protected disk",
/* 0x04 */ "Sector not found",
/* 0x05 */ "Reset failed",
/* 0x06 */ "Disk changed since last operation",
/* 0x07 */ "Drive parameter activity failed",
/* 0x08 */ "DMA overrun",
/* 0x09 */ "Attempt to perform DMA across 64K boundary",
/* 0x0A */ "Bad sector detected",
/* 0x0B */ "Bad track detected",
/* 0x0C */ "Unsupported track",
/* 0x10 */ "Bad CRC/ECC on disk read",
/* 0x11 */ "CRC/ECC corrected data error",
/* 0x20 */ "Controller has failed",
/* 0x40 */ "Seek operation failed",
/* 0x80 */ "Attachment failed to respond",
/* 0xAA */ "Drive not ready",
/* 0xBB */ "Undefined error occurred",
/* 0xCC */ "Write fault occurred",
/* 0xE0 */ "Status error",
/* 0xFF */ "Sense operation failed"
};

void printerr()
{
  fprintf(stderr, "%s\n", strerror(errno));
}

void printerr(const char *format, ...)
{
  va_list argptr;
  va_start(argptr, format);
  vfprintf(stderr, format, argptr);
  va_end(argptr);
}

const char *diskstrerror(const int diskerror)
{
  if ((diskerror >> 8) <= 0x11)
    return diskerrormessage[diskerror >> 8];
  else
    switch (diskerror >> 8)
    {
      case 0x20: return diskerrormessage[0x12];
      case 0x40: return diskerrormessage[0x13];
      case 0x80: return diskerrormessage[0x14];
      case 0xAA: return diskerrormessage[0x15];
      case 0xBB: return diskerrormessage[0x16];
      case 0xCC: return diskerrormessage[0x17];
      case 0xE0: return diskerrormessage[0x18];
      case 0xFF: return diskerrormessage[0x19];
    };
  return diskerrormessage[0x00];
}