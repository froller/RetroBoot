#ifndef _PARTTABL_H_
#define _PARTTABL_H_

#define PARTITION_TABLE_OFFSET 0x01BE
#define PARTITION_TABLE_SIZE sizeof(partentry) * 4
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

#endif