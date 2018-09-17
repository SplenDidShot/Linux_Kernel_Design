/* filename: isr_wrapper.s */
#define ASM     1
#define MIN_SYS_CALL 1 
#define MAX_SYS_CALL 8
#include "isr_wrapper.h"
.text
.globl rtc_wrapper
.globl kbd_wrapper
.globl pit_wrapper
.globl syscall_asm_linkage
.globl halt, execute, read, write, open, close, getargs, vidmap
.align   4
 
rtc_wrapper:
    pushal
    call rtc_handler
    popal
    iret

kbd_wrapper:
    pushal
    call kbd_intr_handler
    popal
    iret

pit_wrapper:
    pushal
    call pit_handler
    popal
    iret

syscall_asm_linkage:
	pushl %ebp
	pushl %esi
	pushl %edi
	

	cmpl $MIN_SYS_CALL, %eax
	jl   invalid_arg
	cmpl $MAX_SYS_CALL, %eax
	jg   invalid_arg

	pushl %edx
	pushl %ecx
	pushl %ebx

	decl %eax
	call *jump_table(,%eax, 4)
	popl %ebx
	popl %ecx
	popl %edx
	popl %edi
	popl %esi
	popl %ebp
	iret

invalid_arg:
	popl %edi
	popl %esi
	popl %ebp
	movl  $-1, %eax                  #return -1 for nonvalid cmd
	iret

jump_table:
	.long halt, execute, read, write, open, close, getargs, vidmap
