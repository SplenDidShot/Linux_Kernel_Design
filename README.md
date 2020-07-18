# Linux_Kernel_Design
This repository contains the source code of my past Linux kernel design project written in x86 and C in a team of four.
Source files are located in the student-distrib directory.

# Components
## Kernel Boot Loading
student-distrib/boot.S/h  
student-distrib/x86_desc.S/h  
student-distrib/isr_wrapper.S/h  

## Interrupt/Terminal IO/System Call Handling
student-distrib/dev_kbd.c/h 
student-distrib/dev_term.c/h  
student-distrib/idt.c/h 
student-distrib/lib.c/h
student-distrib/rtc.c/h 
student-distrib/vga.c/h 

## Filesystem & Memory Structuring
student-distrib/filesystem.c/h
student-distrib/paging.c/h 

## i8259 Chip IRQ Controller
student-distrib/i8259.c/h 

## Multiprocess Scheduler
student-distrib/schedule.c/h 
