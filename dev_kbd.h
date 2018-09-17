#ifndef DEV_KBD_H
#define DEV_KBD_H


#include "lib.h"
#include "x86_desc.h" 
#include "i8259.h"
#include "types.h"
#include "vga.h"
#include "intr_util.h"
#include "dev_term.h"
#include "isr_wrapper.h"
#include "paging.h"
#include "syscalls.h"

#define KBD_STATUS_PORT		0x64
#define KBD_DATA_PORT		0x60
#define KBD_BUF_SIZE		128
#define	KBD_TABLE_SIZE		128	
#define TAB_SIZE			4
#define KBD_OUT_BUF_MASK  1
#define TERM_BUF_SIZE		1024
#define VID_MEM_SIZE		4096
#ifndef ASM




/* initializes kbd handler */
void kbd_init(uint32_t kbd_irq_num);
/* prints character on the terminal based on kbd input */
void kbd_intr_handler();
/* invoked upon backspace*/
void remove_op();
/* invoked upon other keystrokes */
void insert_op(uint8_t c);
/* interprets input scancode */
uint8_t scancode_decoder(uint8_t sc);
/* checks if kbd is ready for output */
uint8_t is_kbd_ready();
/* returns the kbd output buffer */
uint8_t* get_kbd_buf();

void unset_kbd_ready();
/* returns the pcb of current process of the target terminal */
struct pcb* get_term_pcb(uint8_t next_term);
#endif		/* ASM */
#endif		/* DEV_KBD */
