# Linux_Kernel_Design
This repository contains the source code of my past Linux kernel design project written in x86 and C in a team of four.
Source files are located in the student-distrib directory.

# Components
## Kernel Boot Loading
boot.S/h  
x86_desc.S/h  
isr_wrapper.S/h  

## Interrupt/Terminal IO/System Call Handling
dev_kbd.c/h 
dev_term.c/h  
idt.c/h 
lib.c/h
rtc.c/h 
vga.c/h 

## Filesystem & Memory Structuring
filesystem.c/h
paging.c/h 

## i8259 Chip IRQ Controller
i8259.c/h 

## Multiprocess Scheduler
schedule.c/h 
