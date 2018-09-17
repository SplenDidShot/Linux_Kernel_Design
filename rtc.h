#ifndef RTC_H
#define RTC_H

#include "types.h"
#include "lib.h"
// #include "dev_kbd.h"
#include "intr_util.h"
#include "syscalls.h"
#include "isr_wrapper.h"

#define RTC_ADDR_PORT    0x70
#define RTC_DATA_PORT    0x71

#define STATUS_REG_A     0x8A
#define STATUS_REG_B     0x8B     
#define STATUS_REG_C     0x0C

#define PERIODIC_INTERRUPT_MASK  0x40   /*The bit7 of the Register B*/
#define RATE_MASK  	0xF0   
#define RATE       	15
#define FISH_RATE	11

#ifndef ASM
/* initializes rtc handler */
void rtc_init(uint32_t irq_num);
/* prints rtc value on screen */
void rtc_handler();

int32_t rtc_open(const uint8_t* filename);

int32_t rtc_close(int32_t fd);

// int32_t rtc_write(unsigned char* rtc_freq_buf);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);



#endif

#endif // RTC_H
