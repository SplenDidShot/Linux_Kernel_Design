#include "rtc.h"


volatile int next_interrupt;
int cnt = 0;


/*
* rtc_init
* DESC: calls the helper functions that adds kbd entry to the IDT and 
*       enables the kbd IRQ line
* INPUT: kbd_irq_num - IRQ line of the kbd
* OUTPUT: N/A
* RETURN: N/A
* SIDE_EFFECTS: keyboard inputs could be printed on the terminal
*/
void rtc_init(uint32_t irq_num){

	cli();

	next_interrupt = 0;

	outb(STATUS_REG_B, RTC_ADDR_PORT);
	unsigned char prev = inb(RTC_DATA_PORT);
	
	outb(STATUS_REG_B, RTC_ADDR_PORT);
	outb(prev | PERIODIC_INTERRUPT_MASK, RTC_DATA_PORT);
	
	//insert_idt((void*)isr_wrapper((void *)rtc_handler), irq_num);
	insert_idt(rtc_wrapper, irq_num);
	enable_irq(irq_num);
	
	sti();
}

/*
 * rtc_handler
 * DESCRIPTION: keyboard interrupt handler-- prints the corresponding keyboard character on 
 * the terminal according to input
 * I/O: N/A
 * RETURN: N/A
 * SIDE EFFECTS: prints characters and numbers on the console
 */
void rtc_handler(){
	//__asm__("pushal");
	//printf("rtc_handler called");

	/* set freq to 16 interrupts per second */
	
	
	cli();
	next_interrupt = 1;
	// outb( STATUS_REG_A, RTC_ADDR_PORT);	
	// char prev=inb(RTC_DATA_PORT);

	// outb(STATUS_REG_A, RTC_ADDR_PORT);
	// outb((prev & RATE_MASK) | RATE, RTC_DATA_PORT); 
		
	outb(STATUS_REG_C, RTC_ADDR_PORT);	// select register C
	inb(RTC_DATA_PORT);
	 	
// 
	send_eoi(8);
	sti();
	
	//__asm__("popal; leave; iret"); 
}

/*	rtc_open -- initializes the RTC to 2 Hz
*	Input - filename 
*	Output: N/A
*	Return: 0
*	Side Effects: 2 Hz RTC
*/
int32_t rtc_open(const uint8_t* filename){

	rtc_init(8);

	/*initializes RTC frequency to 2HZ*/	
	/*this can be 3 to 15*/
	uint8_t rtc_freq_buf = RATE;

	rtc_write(0, (uint8_t*)(&rtc_freq_buf), 4) ;

	return 0;

}

/*	rtc_close -- turns off the rtc (does nothing)
*	Input: fd - file number
*	Output: N/A
*	Return: 0
*	Side Effects: N/A
*/
int32_t rtc_close(int32_t fd){
	return close(fd);
}

/*	rtc_read - blocks until the next rtc handler call
*	I/O: N/A
*	Return: 0
*	Side Effects: blocks the rtc functions until the next interrupt arrives
*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){

	next_interrupt = 0;
	/*block until the next interrupt*/
	while(next_interrupt == 0);
	return 0;
}

/*	rtc_write - changes the rtc frequency to desired speed
*	Inputs: fd - file number
*			buf - frequency buffer
*	Output: changed rtc frequency
*	Return: -1 on failure, 0 on success
*	Side Effects: changes the rtc frequency
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){

	/*check interrupt rates */
	uint32_t freq = 32768 >> ((*(uint8_t*)buf)-1);

	if(freq<2)
		*(uint8_t*)buf = FISH_RATE;
	else
		*(uint8_t*)buf = RATE;

	/*check frequency must be power of 2 */
	if( (freq & (freq - 1)) != 0)
		return -1;

	/*change the frequency*/
	cli();

	outb(STATUS_REG_A, RTC_ADDR_PORT);	
	char prev=inb(RTC_DATA_PORT);

	outb(STATUS_REG_A, RTC_ADDR_PORT);
	outb((prev & RATE_MASK) | (*(uint8_t*)buf), RTC_DATA_PORT); 
		
	send_eoi(8);
	sti();

	return 0;
}


