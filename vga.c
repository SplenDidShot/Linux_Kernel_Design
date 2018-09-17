#include "vga.h"

static uint8_t addr_reg;



static void set_misc_out_reg();

void vga_reg_init()
{
	set_misc_out_reg();	
}



static void set_misc_out_reg()
{
	uint8_t misc_out_reg;
	misc_out_reg = inb(MISC_OUT_REG_RD);
	misc_out_reg |= 0x1;
	outb(misc_out_reg, MISC_OUT_REG_WR);
}

void disable_cursor()
{
	set_misc_out_reg();

	addr_reg = inb(CRT_AD_REG);	
	outb(CURSOR_START_IDX, CRT_AD_REG);
	outb(0x20, CRT_DT_REG);
	outb(addr_reg, CRT_AD_REG);
}

void enable_cursor()
{
	/* set max scan line */
	//uint8_t max_scan_line_reg;

	/* save current address reg */
	addr_reg = inb(CRT_AD_REG);	
	
	// somehow, it works w/o setting this reg
	/* set max scan line field */
	//max_scan_line_reg &= ~(MAX_SCAN_LINE_MASK);
	//max_scan_line_reg |= (NLINES - 1);
	/* open max scan line reg */
	//outb(MAX_SCAN_LINE_IDX, CRT_AD_REG);
	/* write to max scan line reg */
	//outb(max_scan_line_reg, CRT_DT_REG);

	/* set cursor start reg */
	outb(CURSOR_START_IDX, CRT_AD_REG);
	outb(0, CRT_DT_REG);

	//outb(CURSOR_END_IDX, CRT_AD_REG);
	//outb(NLINES-1, CRT_DT_REG);

	
	/* restore prev address reg */
	outb(addr_reg, CRT_AD_REG);
	
	/* set initial cursor location */
	set_cursor_loc(0);


}

void set_cursor_loc(uint16_t cursor_loc)
{

	uint8_t loc_low;
	uint8_t loc_high;
	loc_low = cursor_loc & 0xff;
	loc_high = cursor_loc >> 8;

	/* save current address reg */
	addr_reg = inb(CRT_AD_REG);	
	
	/* open cursor loc high reg */
	outb(CURSOR_LOC_HIGH_IDX, CRT_AD_REG);
	/* set cursor high addr */
	outb(loc_high, CRT_DT_REG);

	/* open cursor loc low reg */
	outb(CURSOR_LOC_LOW_IDX, CRT_AD_REG);
	/* set cursor low addr */
	outb(loc_low, CRT_DT_REG);

	/* restore prev address reg */
	outb(addr_reg, CRT_AD_REG);
}
