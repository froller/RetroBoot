#include "biosvid.h"

void		_Cdecl biossetmode(
			unsigned char mode
		)
{
  asm {
    mov AH, _VIDEO_SETMODE
    mov AL, mode
    int 0x10
  }
}

void		_Cdecl biosgetmode(
			unsigned char *mode,
                        unsigned char *maxcol,
                        unsigned char *page
		)
{
  unsigned char m, c, p;
  asm {
    mov AH, _VIDEO_GETMODE
    int 0x10
    mov m, AL
    mov c, AH
    mov p, BH
  }
  if (mode)
    *mode = m;
  if (maxcol)
    *maxcol = c;
  if (page)
    *page = p;
}

void		_Cdecl biossetpos(
                        unsigned char  pg,
			unsigned char  row,
                        unsigned char  col
		)
{
  asm {
    mov DH, row
    mov DL, col
    mov BH, pg
    mov AH, _VIDEO_SETPOS
    int 0x10
  }
}

void		_Cdecl biosgetpos(
			unsigned char  pg,
                        unsigned char *row,
                        unsigned char *col
		)
{
  unsigned char r, c;
  asm {
    mov BH, pg
    mov AH, _VIDEO_GETPOS
    int 0x10
    mov r, DH
    mov c, DL
  }
  if (row)
    *row = r;
  if (col)
    *col = c;

}

void		_Cdecl biospage(
			unsigned char  pg
		)
{
  asm {
    mov BH, pg
    mov AH, _VIDEO_PAGE
    int 0x10
  }
}

void		_Cdecl biosgetcha(
			unsigned char	pg,
			unsigned char  *attr,
                        unsigned char  *ch
		)
{
  unsigned char a, c;
  asm {
    mov BH, pg
    mov AH, _VIDEO_GETCHA
    int 0x10
    mov a, AH
    mov c, AL
  }
  if (attr)
    *attr = a;
  if (ch)
    *ch = c;
}

void		_Cdecl biosputch(
			unsigned char  	pg,
                        unsigned short	n,
                        char	  	c
		)
{
  asm {
    mov CX, n
    mov BH, pg
    mov AL, c
    mov AH, _VIDEO_PUTCH
    int 0x10
  }
}

void		_Cdecl biosputcha(
			unsigned char  	pg,
                        unsigned char  	attr,
                        unsigned short 	n,
                        char	  	c
		)
{
  asm {
    mov CX, n
    mov BH, pg
    mov BL, attr
    mov AL, c
    mov AH, _VIDEO_PUTCHA
    int 0x10
  }
}

void		_Cdecl biosputchar(
                        char	  	c
		)
{
  asm {
    mov AL, c
    mov AH, _VIDEO_PUTCHAR
    int 0x10
  }
}

void _Cdecl biosputs(
	unsigned char  pg,
        unsigned char  row,
        unsigned char  col,
        unsigned char  attr,
        unsigned short len,
	const char    *s
)
{
  asm {
    push BP
    mov DH, row
    mov DL, col
    mov CX, len
    mov BH, pg
    mov BL, attr
    mov BP, s
    mov AL, 1
    mov AH, _VIDEO_PUTS
    int 0x10
    pop BP
  }
}

