/* start.S has put the processor into protected 32-bit mode,
	 and set up the right segmentation. The layout of our hard
	 disk is shown below:
	 +-----------+------------------.        .-----------------+
	 | bootblock |  The game binary    ...     (in ELF format) |
	 +-----------+------------------`        '-----------------+
	 So the task of the C code is to load the game binary into
	 correct memory location (0x100000), and jump to it. */

#include "boot.h"

#define SECTSIZE 512
#define KOFFSET  0xC0000000

void readseg(unsigned char *, int, int);

void
bootmain(void) {
	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;
	unsigned char* pa, *i;
	void (*entry)(void);
	unsigned int j;

	/* The binary is in ELF format (please search the Internet).
	   0x8000 is just a scratch address. Anywhere would be fine. */
	elf = (struct ELFHeader*)0x8000;

	/* Read the first 4096 bytes into memory.
	   The first several bytes is the ELF header. */
	readseg((unsigned char*)elf, 4096, 0);

	/* Load each program segment */
	ph = (struct ProgramHeader*)((char *)elf + elf->phoff);
	eph = ph + elf->phnum;
	for(; ph < eph; ph ++) {
		pa = (unsigned char*)(ph->paddr - KOFFSET); /* physical address */
		readseg(pa, ph->filesz, ph->off); /* load from disk */
		for (i = pa + ph->filesz; i < pa + ph->memsz; *i ++ = 0);
	}

	/* Here we go! */
	entry = (void(*)(void))(elf->entry - KOFFSET);
	entry(); /* never returns */
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); /* Spin on disk until ready */
}

/* Read a single sector (512B) from disk */
void
readsect(void *dst, int offset) {
	int i;
	/* Issue command */
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);

	/* Fetch data */
	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

/* Read "count" bytes at "offset" from binary into physical address "pa". */
void
readseg(unsigned char *pa, int count, int offset) {
	unsigned char *epa;
	epa = pa + count;
	pa -= offset % SECTSIZE;
	offset = (offset / SECTSIZE) + 1;
	for(; pa < epa; pa += SECTSIZE, offset ++)
		readsect(pa, offset);
}


/*

0x1f0   :读数据，当0x1f7不为忙状态时，可以读.

0x1f2   :每次读扇区的数目（最小是1）;

0x1f3   :如果是LBA模式就是了LBA参数的0-7位(相当于扇区);

0x1f4   :如果是LBA模式就是了LBA参数的8-15位（相当于柱面）;

0x1f5   :如果是LBA模式就是了LBA参数的16-23位(相当于柱面);

0x1f6   :(第七位必须1，第六位1为LBA模式0为chs模式，第五位必须1，第四位是0为主盘1为从盘，3-0位是LBA的参数27-24位);

0x1f7   :状态命令寄存器，操作时先给命令，在读取，如果不是忙状态的话就从0x1f0读取数据.

*/