#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "lib.h"
#include "types.h"
#include "dev_term.h"
#include "dev_kbd.h"
#include "i8259.h"

#define CMD_REG		0x43
#define MODE2		0x34
#define CHANEL_0 	0x40
#define PIT_IRQ 	0
#define PIT_HZ 		1193182
#define PIT_SHIFT 	8
#define SET_HZ 		20
#define PIT_MASK 	0xff

#define _8KB              (8 << 10)
#define _8MB              (8 << 20)
#define KSTACK_BASE       _8MB - _8KB
#define KSTACK_SIZE       _8KB  


void init_pit();
void pit_handler();
void switch_context(uint32_t pid);


#endif
