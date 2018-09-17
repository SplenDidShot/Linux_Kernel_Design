#include "idt.h"
#include "x86_desc.h"
#include "isr_wrapper.h"

/*	init_idt
*	populates the interrupt descriptor table with proper handling functions
*	I/O: N/A
*	Return: N/A
*	Effects: allows exceptions and interrupts to be handled properly
*
*/
void init_idt(){
	int i;
	for(i=0; i<NUM_VEC; i++)
	{
		 idt[i].offset_15_00 = 0;
		 idt[i].seg_selector = KERNEL_CS;
		 idt[i].reserved4 = 0;
		 idt[i].reserved3 = i<32 ? 0 : 1;
		 idt[i].reserved2 = 1;
		 idt[i].reserved1 = 1;
		 idt[i].size = 1;
		 idt[i].reserved0 = 0;
		 idt[i].dpl = (i==0x80) ? 3 : 0;
		 idt[i].present = ((i<20 && i!=15) || i==0x80 || i==0x20) ? 1 : 0;
		 idt[i].offset_31_16 = 0;
		 if(i!=0x80 || i!=0x20)
		 	SET_IDT_ENTRY(idt[i], exp_default);
		 if(i==0x80)
		 	SET_IDT_ENTRY(idt[i], syscall_asm_linkage);	 	


	}
	SET_IDT_ENTRY(idt[0], exp_de);
	SET_IDT_ENTRY(idt[1], exp_db);
	SET_IDT_ENTRY(idt[2], exp_nmi);
	SET_IDT_ENTRY(idt[3], exp_bp);
	SET_IDT_ENTRY(idt[4], exp_of);
	SET_IDT_ENTRY(idt[5], exp_br);
	SET_IDT_ENTRY(idt[6], exp_ud);
	SET_IDT_ENTRY(idt[7], exp_nm);
	SET_IDT_ENTRY(idt[8], exp_df);
	SET_IDT_ENTRY(idt[9], exp_cso);
	SET_IDT_ENTRY(idt[10], exp_ts);
	SET_IDT_ENTRY(idt[11], exp_np);
	SET_IDT_ENTRY(idt[12], exp_ss);
	SET_IDT_ENTRY(idt[13], exp_gp);
	SET_IDT_ENTRY(idt[14], exp_pf);
	SET_IDT_ENTRY(idt[16], exp_mf);
	SET_IDT_ENTRY(idt[17], exp_ac);
	SET_IDT_ENTRY(idt[18], exp_mc);
	SET_IDT_ENTRY(idt[19], exp_xf);
	SET_IDT_ENTRY(idt[32], pit_wrapper);
}




/* divide by error exp
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_de(){
	printf("Divide by 0 0x0 \n");
	while(1);
}

/* intel reserved exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_db(){
	printf("Intel reserved exception 0x1\n");
	while(1);
}

/* non-maskable interrupt 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_nmi(){
	printf("Non-maskable interrupt invoked 0x2\n");
	while(1);
}

/* breakpoint exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_bp(){
	printf("INT 3(Breakpoint) instruction invoked 0x3\n");
	while(1);
}

/* overflow exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_of(){
	printf("Overflow (INTO) 0x4\n");
	while(1);
}

/* BOUND range exceeded exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_br(){
	printf("Bounds range exceeded 0x5\n");
	while(1);
}

/* Invalid Opcode exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_ud(){
	printf("Invalid opcode 0x6\n");
	while(1);
}

/* Device not available 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_nm(){
	printf("Device not available 0x7\n");
	while(1);
}

/* double fault exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_df(){
	printf("Double fault 0x8\n");
	while(1);
}

/* coprossesor segment overrun exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_cso(){
	printf("Coprocessor segment overrun 0x9\n");
	while(1);
}

/* invalid tss exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_ts(){
	printf("Invalid TSS 0xA\n");
	while(1);
}

/* segment not present exp
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_np(){
	printf("Segment not present 0xB\n");
	while(1);
}

/* stack seg fault exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_ss(){
	printf("Stack-segment fault 0xC\n");
	while(1);
}

/* general protection exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_gp(){
	printf("General protection fault 0xD\n");
	while(1);
}

/* page fault exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_pf(){
	printf("Page fault 0xE\n");
	while(1);
}

/* x87 FPU floating-point error exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_mf(){
	printf("x87 FPU error 0x10\n");
	while(1);
}

/* alignment check exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_ac(){
	printf("Alignment check 0x11\n");
	while(1);
}

/* machine check exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_mc(){
	printf("Machine check 0x12\n");
	while(1);
}

/* simd floating-point exp 
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_xf(){
	printf("SIMD Floating-Point Exception 0x13\n");
	while(1);
}
/* Default/Unknown exp
*  I/O: N/A
*  SIDE EFFECTS: prints message on the terminal upon exception
*/
void exp_default(){
	printf("Unknown exception has occurred\n");
	while(1);
}


