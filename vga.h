#include "lib.h"

#define NLINES						25

/* Miscellaneous Output Register (Read at 3CCh, Write at 3C2h) */
#define MISC_OUT_REG_RD 	0x3CC
#define MISC_OUT_REG_WR 	0x3C2


/* CRT control registers */
#define CRT_AD_REG 		 		0x3D4
#define CRT_DT_REG				0x3D5

/* Maximum Scan Line Register (Index 09h) */
#define MAX_SCAN_LINE_IDX	0x9

#define MAX_SCAN_LINE_MASK 0x1f
/* Cursor Start Register (Index 0Ah) */
#define CURSOR_START_IDX  0xA

/*  Cursor End Register (Index 0Bh) */
#define CURSOR_END_IDX 		0xB
/* Cursor Location High Register (Index 0Eh) */
#define CURSOR_LOC_HIGH_IDX 	0xE


/* Cursor Location Low Register (Index 0Fh) */
#define CURSOR_LOC_LOW_IDX 		0xF


void vga_reg_init();
void disable_cursor();
void enable_cursor();
void set_cursor_loc(uint16_t);

