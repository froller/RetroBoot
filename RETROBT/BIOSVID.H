#ifndef __BIOSVID_H
#define __BIOSVID_H

#define _VIDEO_SETMODE		0x00
#define _VIDEO_SETPOS		0x02
#define _VIDEO_GETPOS		0x03
#define _VIDEO_PAGE		0x05
#define _VIDEO_GETCHA		0x08
#define _VIDEO_PUTCHA		0x09
#define _VIDEO_PUTCH		0x0A
#define _VIDEO_PUTCHAR		0x0E
#define _VIDEO_GETMODE		0x0F
#define _VIDEO_PUTS		0x13

#ifndef ___DEFS_H
#include <_defs.h>
#endif /* ___DEFS_H */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void _Cdecl biossetmode(const unsigned char mode);
void _Cdecl biosgetmode(
  unsigned char *mode,
  unsigned char *maxcol,
  unsigned char *page
  );
void _Cdecl biossetpos(
  const unsigned char pg,
  const unsigned char row,
  const unsigned char col
  );
void _Cdecl biosgetpos(
  const unsigned char pg,
  unsigned char *row,
  unsigned char *col
  );
void _Cdecl biospage(const unsigned char pg);
void _Cdecl biosgetcha(
  const unsigned char pg,
  unsigned char *attr,
  unsigned char *c
  );
void _Cdecl biosputch(
  const unsigned char pg,
  const char c
  );
void _Cdecl biosputcha(
  const unsigned char pg,
  const unsigned char attr,
  const char c
  );
void _Cdecl biosputchar(const char c);
void _Cdecl biosputs(
  const unsigned char pg,
  const unsigned char row,
  const unsigned char col,
  const unsigned char attr,
  const unsigned short len,
  const char *s
  );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BIOSVID_H */
