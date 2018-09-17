/* idt.h - descriptor for idt.c which initializes and populates the interrupt descriptor table
 * 
 *
*/

#ifndef _IDT_H
#define _IDT_H

#include "lib.h"

/* initializes the IDT entries with interrupts and exceptions */
extern void init_idt();

/* divide by error exp */
void exp_de();

/* intel reserved exp */
void exp_db();

/* non-maskable interrupt */
void exp_nmi();

/* breakpoint exp */
void exp_bp();

/* overflow exp */
void exp_of();

/* BOUND range exceeded exp */
void exp_br();

/* Invalid Opcode exp */
void exp_ud();

/* Device not available */
void exp_nm();

/* double fault exp */
void exp_df();

/* coprossesor segment overrun exp */
void exp_cso();

/* invalid tss exp */
void exp_ts();

/* segment not present exp*/
void exp_np();

/* stack seg fault exp*/
void exp_ss();

/* general protection exp */
void exp_gp();

/* page fault exp */
void exp_pf();

/* x87 FPU floating-point error exp */
void exp_mf();

/* alignment check exp */
void exp_ac();

/* machine check exp */
void exp_mc();

/* simd floating-point exp */
void exp_xf();

/* Unkown exception */
void exp_default();


#endif
