#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <string.h>
#include <bios.h>
#include "confirm.h"
#include "error.h"

#define SECTOR_SIZE 512

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printerr("Usage: %s drive file.bin\n\n", argv[0]);
    return 1;
  }
  unsigned char drive = atoi(argv[1]);
  unsigned char *MBR = (unsigned char *)malloc(SECTOR_SIZE);
  FILE *mbr = fopen(argv[2], "rb");
  if (!mbr)
  {
    printerr();
    return 1;
  }
  if (fread(MBR, SECTOR_SIZE, 1, mbr) != 1)
  {
    printerr();
    fclose(mbr);
    free(MBR);
    return 1;
  }
  fclose(mbr);

  unsigned short signature = (unsigned short)MBR[SECTOR_SIZE - 2] << 8 | (unsigned char)MBR[SECTOR_SIZE - 1];
  if (signature != 0x55AA)
  {
    printerr("Invalid signature: 0x%04x\n", signature);
    return 1;
  }

  if (!confirm("Are you sure to write boot sector from %s to drive %u?", argv[2], drive))
  {
    free(MBR);
    return 0;
  }

  int result = biosdisk(_DISK_WRITE, drive, 0, 0, 1, 1, MBR);
  if (result)
  {
    printerr("%s\n", diskstrerror(result >> 8));
    free(MBR);
    return 1;
  }

  free(MBR);
  return 0;
}
