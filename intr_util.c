#include "intr_util.h"
/*
* insert_idt
* DESC: inserts IDT entry at runtime while initializing keyboard driver
* INPUTS: handler - function ptr to the interrupt handler code segment
*		  irq_num - the target interrupt line that is being initialized
* OUTPUT: N/A
* RETURN: 0 on success, -1 on failure
* SIDE_EFFECTS: complements the IDT
*/
int insert_idt(void (*handler)(), uint32_t irq_num) {

	if (handler == NULL) {
		printf("No handler found!");
		return -1;
	}
	SET_IDT_ENTRY(idt[RESV_IDT_NUM+irq_num], (uint32_t)handler);
	idt[RESV_IDT_NUM+irq_num].present = 1;
	idt[RESV_IDT_NUM+irq_num].seg_selector = KERNEL_CS;

	return 0;

}
