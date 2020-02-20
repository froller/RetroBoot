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

void		_Cdecl biossetmode(
			unsigned char   mode
		);
void		_Cdecl biosgetmode(
			unsigned char  *mode,
                        unsigned char  *maxcol,
                        unsigned char  *page
		);
void		_Cdecl biossetpos(
                        unsigned char   pg,
			unsigned char   row,
                        unsigned char   col
		);
void		_Cdecl biosgetpos(
			unsigned char  	pg,
                        unsigned char  *row,
                        unsigned char  *col
		);
void		_Cdecl biospage(
			unsigned char  	pg
		);
void		_Cdecl biosgetcha(
			unsigned char	pg,
			unsigned char  *attr,
                        unsigned char  *c
		);
void		_Cdecl biosputch(
			unsigned char  	pg,
                        char	  	c
		);
void		_Cdecl biosputcha(
			unsigned char  	pg,
                        unsigned char  	attr,
                        char	  	c
		);

void		_Cdecl biosputchar(
                        char	  	c
		);
void		_Cdecl biosputs(
			unsigned char   pg,
                        unsigned char   row,
                        unsigned char   col,
                        unsigned char   attr,
                        unsigned short  len,
			const char     *s
		);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BIOSVID_H */
