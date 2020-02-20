#ifndef _ERROR_H_
#define _ERROR_H_

extern int diskerror;

void printerr();
void printerr(const char *format, ...);

const char *diskstrerror(const int diskerror);

#endif