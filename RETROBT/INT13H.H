#ifndef __INT13H_H
#define __INT13H_H

#ifdef __cplusplus
#  define INTARGS ...
extern "C" {
#else
#  define INTARGS unsigned bp, unsigned di, unsigned si,\
                  unsigned ds, unsigned es, unsigned dx,\
                  unsigned cx, unsigned bx, unsigned ax
#endif // __cplusplus

extern char fakeMBR[512];
extern unsigned char fakePartition;
extern unsigned char fakeDrive;
extern void interrupt (far *biosInt13h)(INTARGS);
extern void interrupt int13h(INTARGS);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INT13H_H
