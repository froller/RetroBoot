#ifndef __PARTENTR_H
#define __PARTENTR_H

#define PRTTBL_OFFSET 0x01BE
#define PRTTBL_SIZE sizeof(partentry) * 4
#define PRTTBL_SIGNATURE 0xAA55

struct partchs
{
  unsigned char  head;
  unsigned char  sec: 6;
  unsigned char  cylhi:2;
  unsigned char  cyllo;
  unsigned short getCyl()
  {
    return ((unsigned short)cylhi << 8) | cyllo;
  }
  void setCyl(const unsigned short cyl)
  {
    cyllo = cyl & 0x00FF;
    cylhi = cyl & 0x0300 << 8;
  }
};

struct partentry
{
  unsigned char active;
  struct partchs startchs;
  unsigned char type;
  struct partchs endchs;
  unsigned long startlba;
  unsigned long sizelba;
};

struct parttable
{
  struct partentry partition[4];
  unsigned short signature;
};

#endif
