/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
volatile uint8_t master_mask; /* IRQs 0-7  */
volatile uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */

 /*
  * i8259_init(void)
  * DESCRIPTION: Initialize the 8259 PIC
  * I/O: N/A
  * RRTURN VALUE: none
  * SIDE EFFECT: PIC initialized
  **/
void i8259_init(void) {

	master_mask = 0xff;
	slave_mask = 0xff;

	outb(master_mask, MASTER_8259_CMD_PORT);	/* mask all of 8259A-1 */
	outb(slave_mask, SLAVE_8259_CMD_PORT);	/* mask all of 8259A-2 */

	outb(ICW1, MASTER_8259_PORT);	/* ICW1: select 8259A-1 init */
	outb(ICW2_MASTER, MASTER_8259_CMD_PORT);	/* ICW2: high bit of vector # */
	outb(ICW3_MASTER, MASTER_8259_CMD_PORT);	/* ICW3: bit vector of slave */
	outb(ICW4, MASTER_8259_CMD_PORT);	/* ICW4: ISA=x86 */

	outb(ICW1, SLAVE_8259_PORT);	/* ICW1: select 8259A-2 init */
	outb(ICW2_SLAVE, SLAVE_8259_CMD_PORT);	/* ICW2: high bit of vector # */
	outb(ICW3_SLAVE, SLAVE_8259_CMD_PORT);	/* ICW3: bit vector of slave */
	outb(ICW4, SLAVE_8259_CMD_PORT);	/* ICW4: ISA=x86 */


}

/* enable_irq 
* DESC: Enable (mask) the specified IRQ 
* INPUT: irq_num - the IRQ port that's being enabled
* OUTPUT: NONE
* RETURN: NONE
* SIDE EFFECTS: enables the interrupt line to receive interrupts
*/
void enable_irq(uint32_t irq_num) {
	//masking the unwanted bits
	//uint8_t mask;
	uint8_t mask = ~(1 << (irq_num & 7));
	
	uint8_t IRQ2 = 2;
	if (irq_num >= IRQSEQ) {
		//mask = ~(1 << (irq_num - IRQSEQ));
		master_mask &= ~(1 << IRQ2);
		slave_mask &= mask ;

		outb(master_mask, MASTER_8259_CMD_PORT);
		outb(slave_mask, SLAVE_8259_CMD_PORT);
	} 
	else {
		//mask = ~(1 << irq_num);
		master_mask &= mask;
		outb(master_mask, MASTER_8259_CMD_PORT);
	}

}

/* disable_irq 
* DESC: Disable (mask) the specified IRQ 
* INPUT: irq_num - the IRQ port that's being disabled
* OUTPUT: NONE
* RETURN: NONE
* SIDE EFFECTS: masks the interrupt line to disable interrupts
*/
void disable_irq(uint32_t irq_num) {
	//masking last three bits
	uint8_t mask = (1 << (irq_num & 7));
	uint8_t IRQ2 = 2;
	if (irq_num >= IRQSEQ) {
		//mask = 1 << (irq_num - IRQSEQ);
		master_mask |= (1 << IRQ2);
		slave_mask |= mask ;
		outb(master_mask, MASTER_8259_CMD_PORT);
		outb(slave_mask, SLAVE_8259_CMD_PORT);
	} 
	else {
		mask = 1 << irq_num;
		master_mask |= mask;
		outb(master_mask, MASTER_8259_CMD_PORT);
	}

}

/* send_eoi 
* DESC: Send end-of-interrupt signal for the specified IRQ 
* INPUT: irq_num - the IRQ port that's sending the EOI
* OUTPUT: NONE
* RETURN: NONE
* SIDE EFFECTS: returns from the current interrupt
*/
void send_eoi(uint32_t irq_num) {
	//second IRQ port of the master
	uint8_t IRQ2 = 2;

	if (irq_num >= IRQSEQ) {

		outb(EOI | (irq_num - IRQSEQ), SLAVE_8259_PORT);
		outb(EOI | IRQ2, MASTER_8259_PORT);
	} 
	else {

		outb(EOI | irq_num, MASTER_8259_PORT);
	}


}
