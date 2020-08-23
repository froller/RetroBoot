#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <bios.h>
#include "biosvid.h"
#include "bprintf.h"
#include "parttabl.h"

#define MAXDISK 8
#define MAXOPTION 16
#define SECTOR_SIZE 512

struct disk {
  unsigned char id;
  unsigned char drvtype;
  unsigned char heads;
  unsigned short cylinders;
  unsigned char sectors;
  struct partentry *partitions;
};

#pragma pack(push)
#pragma pack(1)
struct fsinfo {
  char OEMID[9];
  unsigned long serialNumber;
  char label[12];
  char FSType[9];
//  unsigned char fsSPT;
//  unsigned char fsHeads;
};
#pragma pack(pop)

struct bootoption {
  unsigned char drive;
  char letter;
  struct partentry *partition;
  struct fsinfo *fs;
};

char const *diskerrormessage[] = {
/* 0x00 */ "No error",
/* 0x01 */ "Bad command",
/* 0x02 */ "Address mark not found",
/* 0x03 */ "Disk is write-protected",
/* 0x04 */ "Sector not found",
/* 0x05 */ "Reset failed",
/* 0x06 */ "Disk changed since last operation",
/* 0x07 */ "Drive parameter activity failed",
/* 0x08 */ "DMA overrun",
/* 0x09 */ "Attempt to perform DMA across 64K boundary",
/* 0x0A */ "Bad sector detected",
/* 0x0B */ "Bad track detected",
/* 0x0C */ "Unsupported track",
/* 0x10 */ "Bad CRC/ECC on disk read",
/* 0x11 */ "CRC/ECC corrected data error",
/* 0x20 */ "Controller has failed",
/* 0x40 */ "Seek operation failed",
/* 0x80 */ "Attachment failed to respond",
/* 0xAA */ "Drive not ready",
/* 0xBB */ "Undefined error occurred",
/* 0xCC */ "Write fault occurred",
/* 0xE0 */ "Status error",
/* 0xFF */ "Sense operation failed"
};

#if defined(DEBUG)
void dumpPartitionTable(struct partentry table[4]);
#endif

int diskGetConfig(unsigned char drive, struct disk *config);
struct disk *findDisks();
int loadPartitions(
  struct partentry    *table,
  const unsigned char  drive,
  const unsigned char  head,
  const unsigned short track,
  const unsigned char  sector,
  unsigned char        n = 0
);
struct fsinfo *loadFSInfo(
  const unsigned char  drive,
  const unsigned char  head,
  const unsigned short track,
  const unsigned char  sector
);
void loadBootOptions(struct bootoption *b);
void showBootOptions(const struct bootoption *b, const unsigned char s = 0);
void printBootOption(const struct bootoption *b, const unsigned char s = 0);
void useBootOption(const struct bootoption *b);
char *strip(char *str);
void halt(void);
void boot(
  const unsigned char  drive,
  const unsigned char  head,
  const unsigned short track,
  const unsigned char  sector
);


struct bootoption options[MAXOPTION];

int main(int /*argc*/, char ** /*argv*/)
{
#if defined(DEBUG)
  bprint("debug");
#else
  bprint("ok");
#endif
  bprint("\r\n\n");

  unsigned char row, col;
  biosgetpos(0, &row, &col);
  loadBootOptions(options);

  int s = 0;

  while (1) {
    biossetpos(0, row, col);
    showBootOptions(options, s);
    while (!bioskey(1));
    switch (bioskey(0))
    {
    case 0x4800:
      --s;
      break;
    case 0x5000:
      ++s;
      break;
#if defined(DEBUG)
    case 0x011b:
      abort();
      break;
#endif
    case 0x1c0d:
    case 0xe00d:
      useBootOption(&options[s]);
      return 0;
    }
    if (s < 0)
      ++s;
    if (!options[s].letter)
      --s;
  };
}

/****************************************************************************
*
* Displays boot option selector
*
****************************************************************************/

void showBootOptions(const struct bootoption *b, const unsigned char s)
{
  for (int i = 0; i < MAXOPTION && b[i].letter; ++i)
    printBootOption(&b[i], s == i);
}

/****************************************************************************
*
* Prints single boot option to screen
*
****************************************************************************/

void printBootOption(const struct bootoption *b, const unsigned char s)
{
  int bytes; // Length of printed boot option
  unsigned char attr = s ? 0x70 : 0x0F;	// Choose attribute
  bytes = bprintaf("  %c: %-11s (HDD%u, %s, %lu MB)", attr,
    b->letter, b->fs->label,
    b->drive & 0x7F, strip(b->fs->FSType), b->partition->sizelba / 2000
  );
  // Spaces hightlighted
  for (int i = bytes; i < 48; ++i)
    bprinta(" ", attr);
  // Spaces to the end of line
  for (; i < 80; ++i)
    bprinta(" ", 0x07);
}

/****************************************************************************
*
* Just wrapper around boot() to minimize number of arguments
*
****************************************************************************/

void useBootOption(const struct bootoption *b)
{
  boot(
    b->drive,
    b->partition->startchs.head,
    b->partition->startchs.getCyl(),
    b->partition->startchs.sec
  );
}

/****************************************************************************
*
* Analyzes disks and creates set of boot options
*
****************************************************************************/

void loadBootOptions(struct bootoption *b)
{
  struct disk *disks = findDisks();
  char letter = 'C';
  char oi = 0;

  // Cycle through found physical disks
  // to enumerate first partitions of each one
  for (int d = 0; disks[d].drvtype; ++d)
  {
    // Load partition table
    disks[d].partitions
      = (struct partentry *)malloc(sizeof(struct partentry) * 16);
    loadPartitions(disks[d].partitions, disks[d].id, 0, 0, 1, 0);
#if defined(DEBUG) && 0
    bprintf("Partitios on drive %d\r\n", disks[d].id);
    dumpPartitionTable(disks[d].partitions);
#endif

    // Load filesystem info of 1st partition
    struct fsinfo *fsinfo = loadFSInfo(
      disks[d].id,
      disks[d].partitions[0].startchs.head,
      disks[d].partitions[0].startchs.getCyl(),
      disks[d].partitions[0].startchs.sec
    );

    // Fill boot option for 1st partition
    // DOS enumerates first partitions of all available
    // physical disks at first
    // Then other partitions in order of appearance
    b[oi].drive  = disks[d].id;
    b[oi].letter = letter++;
    b[oi].partition = &disks[d].partitions[0];
    b[oi].fs = fsinfo;
    oi++;
  }
  // Cycle through found physical disks again
  // to enumerate rest partitions
  for (d = 0; disks[d].drvtype; ++d)
    // Cycle through partitions starting from 2nd
    for (int i = 1; disks[d].partitions[i].type; ++i)
    {
      // Load filesystem info of i-th partition
      struct fsinfo *fsinfo = loadFSInfo(
        disks[d].id,
        disks[d].partitions[i].startchs.head,
        disks[d].partitions[i].startchs.getCyl(),
        disks[d].partitions[i].startchs.sec
      );
      // Fill boot option
      b[oi].drive  = disks[d].id;
      b[oi].letter = letter++;
      b[oi].partition = &disks[d].partitions[i];
      b[oi].fs = fsinfo;
      oi++;
    }
  // Add terminating record
  if (oi < MAXOPTION)
    memset(&b[oi], 0, sizeof(struct bootoption));
}

/****************************************************************************
*
* Gets drive config by id
*
****************************************************************************/

int diskGetConfig(unsigned char drive, struct disk *config)
{
  asm {
    mov DL, drive
    mov AH, 0x08
    int 0x13
    jc Error
    push AX
    push BX
    pop AX
    mov BX, config
    mov [BX].drvtype, AL
    inc DH
    mov [BX].heads, DH
    push CX
    mov AL, CL
    and AL, 0x3F
    mov [BX].sectors, AL
    mov AH, CL
    and AH, 0xC0
    mov CL, 6
    ror AH, CL
    pop CX
    mov AL, CH
    inc AX
    mov [BX].cylinders, AX
    mov AL, [drive]
    mov [BX].id, AL
    xor AX, AX
    mov [BX].partitions, AX
    pop AX
  }
Error:
  asm {
    pop BP
    ret
  }
  return 0;
}

/****************************************************************************
*
* Find disks
*
****************************************************************************/

struct disk *findDisks()
{
  struct disk *disks
    = (struct disk *)malloc(sizeof(struct disk));
  int disksFound = 0;
  for (int i = 0; i < MAXDISK; ++i)
    if (!diskGetConfig(0x80 + i, disks + disksFound))
    {
      disksFound++;
      disks = (struct disk *)realloc(
        disks,
	sizeof(struct disk) * (disksFound + 1)
      );

    }
  // Fill last element with zeroes
  memset(&disks[disksFound], 0, sizeof(struct disk));
  return disks;
}

/****************************************************************************
*
* Loads partition table from specified sector
*
****************************************************************************/

int loadPartitions(
  struct partentry    *table,
  const unsigned char  drive,
  const unsigned char  head,
  const unsigned short track,
  const unsigned char  sector,
  unsigned char        n
)
{
  // Load first sector
  char sectorBuf[SECTOR_SIZE];
  unsigned short result	= biosdisk(_DISK_READ, drive, head, track, sector, 1, sectorBuf);
  if (result >> 8)
  {
    bprint(diskerrormessage[result >> 8]);
    halt();
  }

  // Decoding partition table from sectorBuf
  struct parttable *p
    = (struct parttable *)&sectorBuf[PRTTBL_OFFSET];

  // Checking signature
  if (p->signature != PRTTBL_SIGNATURE)
  {
    bprint("Wrong signature. MBR damaged.\r\n");
    halt();
  }

  // Analyzing partition table
  for (int i = 0; i < 4; ++i)
  {
    switch (p->partition[i].type)
    {
    // Do nothing on empty record
    case 0x00:
      break;
    // Recurse on extended partition
    case 0x05:
      n = loadPartitions(
      	table,
	drive,
	p->partition[i].startchs.head,
	p->partition[i].startchs.getCyl(),
	p->partition[i].startchs.sec,
	n
      );
      break;
    // Copy partition table records
    default:
      memcpy(&table[n++], &p->partition[i], sizeof(struct partentry));
    }
  }
  // Fill last element with zeroes
  memset(&table[n], 0, sizeof(struct partentry));
  return n;
}

/****************************************************************************
*
* Loads filesystem info from specified sector
*
****************************************************************************/

struct fsinfo *loadFSInfo(
  const unsigned char  drive,
  const unsigned char  head,
  const unsigned short track,
  const unsigned char  sector
)
{
  char sectorBuf[SECTOR_SIZE];
  unsigned short result	= biosdisk(_DISK_READ, drive, head, track, sector, 1, sectorBuf);
  if (result >> 8)
  {
    bprint(diskerrormessage[result >> 8]);
    halt();
  }
  struct fsinfo *fsinfo = (struct fsinfo *)malloc(sizeof(struct fsinfo));
  memset(fsinfo, 0, sizeof(struct fsinfo));
  memcpy(&fsinfo->OEMID,        sectorBuf + 0x03, 8);
  memcpy(&fsinfo->serialNumber, sectorBuf + 0x27, 4);
  memcpy(&fsinfo->label,        sectorBuf + 0x2B, 11);
  memcpy(&fsinfo->FSType,       sectorBuf + 0x36, 8);
//  fsinfo->fsSPT   = *(unsigned short *)(sectorBuf + 0x18);
//  fsinfo->fsHeads = *(unsigned short *)(sectorBuf + 0x1A);
  return fsinfo;
}

/****************************************************************************
*
* Strips trailing spaces
*
****************************************************************************/

char *strip(char *str)
{
  for (int i = strlen(str) - 1; i >= 0; --i)
    if (str[i] == ' ')
      str[i] = 0;
  return str;
}

/****************************************************************************
*
* Hangs system
*
****************************************************************************/

void halt(void)
{
  bprintf("Boot failed. Returning to BIOS.\r\n");
  asm {
    int 0x18
  }
}

/****************************************************************************
*
* Loads specified boot sector in memory and executes boot procedure
*
****************************************************************************/

void boot(
  const unsigned char  drive,
  const unsigned char  head,
  const unsigned short track,
  const unsigned char  sector
)
{
  unsigned char e;
  asm {
    xor AX, AX
    mov ES, AX
    mov DS, AX
    mov BX, 0x7C00
    push BX
    mov AH, 0x02
    mov AL, 0x01
    mov DX, track
    mov CL, 6
    shl DH, CL
    or DH, sector
    mov CX, DX
    xchg CH, CL
    xor DX, DX
    mov DL, drive
    push DX
    mov DH, head
    int 0x13
    jc Error
    xor AX, AX
  }
Error:
  asm {
    mov e, AH
  }
  if (e) {
    bprint(diskerrormessage[e]);
    halt();
  }
  asm {
    pop DX
    pop BX

    // Hack jump instruction +0x0A to -0x02
    // To hang system instead of falling
    // to Non-system disk or disk error
#if defined(DEBUG) && defined(HANG_NO_SYSTEM)
    mov CL, 0xFE
    mov [BX+0xE2], CL
#endif // DEBUG && HANG_NO_SYSTEM

    mov [BX+0x24], DL	// Hacking physical drive number
    mov AX, 0xAA55
    cmp [BX+0x01FE], AL
    jne NoSignature

    DB 0x00EA 		// jmp far
    DW 0x7C00, 0x0000	// 0000:7C00
  }
NoJump:
  bprint("No JMP instruction.\r\n");
  halt();
NoSignature:
  bprint("Wrong signature.\r\n");
  halt();
}

#if defined(DEBUG)
void dumpPartitionTable(struct partentry table[4])
{
  for (int i = 0; i < 4 && table[i].type; i++)
    bprintf("%i %c 0x%02x %i/%i/%i %i/%i/%i\r\n",
      i,
      table[i].active & 0x80 ? '*' : ' ',
      table[i].type,
      table[i].startchs.head,
      table[i].startchs.getCyl(),
      table[i].startchs.sec,
      table[i].endchs.head,
      table[i].endchs.getCyl(),
      table[i].endchs.sec
    );

}
#endif