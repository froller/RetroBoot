#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <bios.h>
#include "partentr.h"
#include "error.h"
#include "confirm.h"

#define SECTOR_SIZE 512
#define SIGNATURE_SIZE 2
#define SIGNATURE 0xAA55

void copyPartTable(const void *from, void *to);
void dumpPartTable(const void *buffer);
int  checkSignature(const void *buffer);
void writeSignature(void *buffer);
int  getSectorsPerTrack(const unsigned char drive);
void usage(char *argv[]);

int drive = 0;
unsigned int result;

char   stage1[SECTOR_SIZE] = { 0 };
FILE  *stage1File = NULL;
char  *stage1FileName = NULL;
size_t stage1FileSize = 0;
const size_t stage1Size = SECTOR_SIZE;

char  *stage15 = NULL;
FILE  *stage15File = NULL;
char  *stage15FileName = NULL;
size_t stage15FileSize = 0;
size_t stage15Size = 0;
size_t stage15DiskSize = 0;

char   bootSector[SECTOR_SIZE] = { 0 };

struct stat statbuf;

int main(int argc, char *argv[])
{
  //
  // Разбор параметров
  //
  if (argc < 3 || 4 < argc)
    usage(argv);

  int drive;
  if (sscanf(argv[1], "%u", &drive) != 1)
    usage(argv);
  //if (3 < drive || drive < 0)
  //  usage(argv);

  stage1FileName = strdup(argv[2]);
  if (argc == 4)
    stage15FileName = strdup(argv[3]);

  //
  // Чтение оригинального загрузочного сектора
  //
  result = biosdisk(_DISK_READ, drive, 0, 0, 1, 1, bootSector);
  if (result)
  {
    printerr("%s", diskstrerror(result >> 8));
    exit(1);
  }
  else
    printf("MBR read\n");

  //
  // Проверка сигнатуры
  //
  if (!checkSignature(bootSector))
  {
    printerr("Invalid signature. MBR is damaged!");
    exit(1);
  }
  else
    printf("Signature ok\n");

  //
  // Загрузка Stage 1 из файла
  //
  if (stat(stage1FileName, &statbuf))
  {
    printerr();
    exit(1);
  }
  stage1FileSize = statbuf.st_size;
  stage1File = fopen(stage1FileName, "rb");
  if (!stage1File)
  {
    printerr();
    exit(1);
  }
  if (!fread(stage1, stage1FileSize, 1, stage1File))
  {
    printerr();
    fclose(stage1File);
    exit(1);
  }
  fclose(stage1File);

  //
  // Внедрение таблицы разделов в Stage 1
  //
  copyPartTable(bootSector, stage1);

  //
  // Запись сигнатуры в Stage 1
  //
  writeSignature(stage1);

  //
  // Загрузка Stage 1.5 из файла
  //
  if (stage15FileName) {
    if (stat(stage15FileName, &statbuf))
    {
      printerr();
      exit(1);
    }
    stage15FileSize = statbuf.st_size;
    stage15DiskSize = 0;
    while (stage15DiskSize < stage15FileSize)
      stage15DiskSize += SECTOR_SIZE;
    stage15 = (char *)malloc(stage15DiskSize);
    if (!stage15)
    {
      printerr();
      exit(1);
    }
    memset(stage15, 0, stage15DiskSize);
    stage15File = fopen(stage15FileName, "rb");
    if (!stage15File)
    {
      printerr();
      exit(1);
    }
    if (!fread(stage15, stage15FileSize, 1, stage15File))
    {
      printerr();
      fclose(stage15File);
      exit(1);
    }
    fclose(stage15File);
  }

  //
  // Подтверждение
  //
  if (!confirm("Are you sure to write bootloader from %s to drive %u?", argv[2], drive & 0x7F))
  {
    printf("Wise decision :-|\n");
    free(stage1);
    return 0;
  }

  if (stage15FileName)
  {
    size_t n = stage15DiskSize / SECTOR_SIZE;

    //
    // Запись нового Stage 1.5 в сектора 2-63
    //
    result = biosdisk(_DISK_WRITE, drive, 0, 0, 2, n, stage15);
    if (result)
    {
      printerr("%s", diskstrerror(result >> 8));
      exit(1);
    }
    else
      printf("Stage 1.5 written (%i sectors used)\n", n);

  }

  //
  // Запись нового Stage 1 в загрузочный сектор
  //
  result = biosdisk(_DISK_WRITE, drive, 0, 0, 1, 1, stage1);
  if (result)
  {
    printerr("%s", diskstrerror(result >> 8));
    exit(1);
  }
  else
    printf("Stage 1 written\n");

  //free(BIN);
  return 0;
}

void copyPartTable(const void *from, void *to)
{
  memcpy(
    (char *)to + PARTITION_TABLE_OFFSET,
    (char *)from + PARTITION_TABLE_OFFSET,
    PARTITION_TABLE_SIZE
  );
}

int  checkSignature(const void *buffer)
{
  unsigned short *sigPtr = (unsigned short *)((char *)buffer + SECTOR_SIZE - SIGNATURE_SIZE);
  return (unsigned short)*sigPtr == SIGNATURE;
}

void writeSignature(void *buffer)
{
  unsigned short *sigPtr = (unsigned short *)((char *)buffer + SECTOR_SIZE - SIGNATURE_SIZE);
  *sigPtr = SIGNATURE;
}

void dumpPartTable(const void *mbr)
{
  struct partentry *partentry = (struct partentry *)((char *)mbr + PARTITION_TABLE_OFFSET);
  for (int i = 0; i < 4; ++i)
  {
    if (partentry[i].type)
    {
      printf("Partition\t: %i\n", i);
      printf("Type\t\t: 0x%02x\t\tActive\t\t: %s\n",
      	partentry[i].type,
	partentry[i].active ? "Yes" : "No");
      printf("Start (CHS)\t: %u %2u %2u\tEnd (CHS)\t: %u %2u %2u\n",
      	partentry[i].startchs.getCyl(),
	partentry[i].startchs.head,
	partentry[i].startchs.sec,
      	partentry[i].endchs.getCyl(),
	partentry[i].endchs.head,
	partentry[i].endchs.sec);
      printf("Start (LBA)\t: %u\t\tSize (LBA)\t: %u\n",
      	partentry[i].startlba,
	partentry[i].sizelba);
      printf("Size (MB)\t: %u\n\n", partentry[i].sizelba >> 11);
    }
  }
}

void usage(char *argv[])
{
  printerr("Usage: %s diskNo stage1.bin [stage15.bin]\n", argv[0]);
  exit(1);
}
