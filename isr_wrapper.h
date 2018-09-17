#ifndef ISR_WRAPPER_H
#define ISR_WRAPPER_H


#ifndef ASM

#include "rtc.h"
#include "dev_kbd.h"
#include "syscalls.h"
#include "schedule.h"

void rtc_wrapper();
void kbd_wrapper();
void pit_wrapper();
int32_t syscall_asm_linkage();

#endif
#endif
