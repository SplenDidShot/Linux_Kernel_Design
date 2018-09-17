#ifndef INTR_UTIL_H
#define INTR_UTIL_H

#include "lib.h"
#include "x86_desc.h" 
#include "i8259.h"
#include "types.h"

#define RESV_IDT_NUM 	32

/* inserts interrupt desc entry in the IDT */
extern int insert_idt(void (*handler)(), uint32_t irq_num);
#endif
