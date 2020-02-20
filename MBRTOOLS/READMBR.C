#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <bios.h>
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
  memset(MBR, 0, SECTOR_SIZE);

  int result = biosdisk(_DISK_READ, drive, 0, 0, 1, 1, MBR);
  if (result)
  {
    printerr("%s\n", diskstrerror(result >> 8));
    free(MBR);
    return 1;
  }

  FILE *mbr = fopen(argv[2], "wb");
  if (!mbr)
  {
    printerr();
    free(MBR);
    return 1;
  }
  if (fwrite(MBR, SECTOR_SIZE, 1, mbr) != 1)
  {
    printerr();
    fclose(mbr);
    free(MBR);
    return 1;
  }
  fclose(mbr);

  free(MBR);
  return 0;
}
