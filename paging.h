#ifndef PAGING_H
#define PAGING_H
#include "x86_desc.h"
#include "dev_term.h"
#include "dev_kbd.h"


#define NUM_ENTRY 1024
#define PAGE_SIZE 4096
#define BLANK 0x00000002
#define PAGE_SHIFT 12
#define TAB_SHIFT 22
#define VMEM	0xB8000
#define RW_P_MASK	3
#define PAGE_MASK	0x80000000
#define KERNEL_PHYS_VIRT_MEMO_ADDR 0x400000
#define FOUR_MB_PAGE 0x80
#define FOUR_MB 0x400000
#define PROGRAM_PAGE_MASK 0x87
#define PROGRAM_IDX 32

#define VID_MEM_INIT 0x8800000
#define VID_FOURKB 0x00017
#define PD_MASK			 	 0xFFC00000
#define PT_MASK			 	 0x003FF000

void paging_setup();
void set_up_program_paging(int32_t process_number);
void remap_terminal_schedule(int32_t virtual_addr, int32_t phy_addr );
uint8_t* vidmap_helper();
//void unmap_term(uint8_t term_flag);
void map_term(uint8_t term_flag, uint8_t old_flag, int32_t* addr);

#endif
