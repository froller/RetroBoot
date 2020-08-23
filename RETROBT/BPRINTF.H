#ifndef __BPRINTF_H
#define __BPRINTF_H

#ifndef ___DEFS_H
#include <_defs.h>
#endif // ___DEFS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void _Cdecl bprinta(const char *str, const unsigned char attr);
void _Cdecl bprint(const char *str);
int _Cdecl bprintaf(const char *fmt, const unsigned char attr, ...);
int _Cdecl bprintf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __BPRINTF_H