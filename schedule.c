#include "schedule.h"

uint8_t pit_counter=0;
uint8_t curr_exec_term = 0;
uint8_t next_exec_term;
extern uint8_t term_flag;
extern term_t term_arr[3];
/*   pit_handler
*    gets invoked by the PIT IRQ to periodically interrupt and run programs on different shells
*    I/O: N/A
*    Return: N/A
*    Effects: enables scheduling to work
*/
void pit_handler(){
  curr_exec_term = curr_exec_term%3;
 
  cli();
  send_eoi(PIT_IRQ);
  next_exec_term  = curr_exec_term;
  //pcb_t* temp_pcb = get_term_pcb(curr_exec_term);
  //printf("cc");
 // printf("curr_exec_term is %d \n",curr_exec_term);
  if(term_arr[next_exec_term].term_pcb!=NULL){
   // switch_context(temp_pcb->pid);
  }
 // printf("bb");
  curr_exec_term++;

  sti();
  return ;
}

/*   switch_context
*    gets called by the pit handler to further carry out scheduling functions
*    Inputs: next_proc - next process number to be ran by the OS
*    Output: N/A
*    Return: N/A
*    Effects: runs programs on different terminals in equal cycles
*/
void switch_context(uint32_t next_proc){
  //printf("curr_exec_term is %d \n",curr_exec_term);
  if(term_arr[curr_exec_term].activated ==0) return ;
  if(term_arr[curr_exec_term].term_pcb ==NULL) return ;
  if((term_arr[0].num_program+term_arr[1].num_program+term_arr[2].num_program)<4) return ;

  set_up_program_paging(next_proc);
  //printf("pf test1 \n");
  uint8_t * screen_start;
  vidmap(&screen_start); //remap vid mem to 136MB
  //printf("pf test2 \n");
    /* Local variables */
    pcb_t* old_pcb;
    pcb_t* next_pcb;
    old_pcb = get_pcb();
   // printf("pf test3 \n");
    next_pcb = (pcb_t*)PCB_BASE_ADDR-PCB_BLOCK*(next_proc+1); //??????????need +1 or not 
    //current executing terminal is not the showing one 
    //if(next_exec_term != term_flag){
     // remap_terminal_schedule((uint32_t)screen_start, (uint32_t)term_arr[curr_exec_term].vid_buf); //first virtual memory,second physical memo.
   // printf("pf test4 \n");
   // }
    asm volatile(
               "movl %%esp, %%eax;"
               "movl %%ebp, %%ebx;"
               :"=a"(old_pcb->esp), "=b"(old_pcb->ebp)    /* outputs */
               :                                          /* no input */
               );

    tss.ss0 = KERNEL_DS;
    tss.esp0 = KSTACK_BASE - KSTACK_SIZE * (next_proc)-4;


               
  asm volatile(
               "movl %%eax, %%esp;"
               "movl %%ebx, %%ebp;"
               :                                          /* no outputs */
               :"a"(next_pcb->esp), "b"(next_pcb->ebp)    /* input */
               );
  return ;
  }

//reference webpage:https://wiki.osdev.org/Programmable_Interval_Timer#The_Oscillator
// oscillator used by the PIT chip runs at (roughly) 1.193182 MHz.//1193182 除以多少 就是多少hz
//The PIT has only 16 bits that are used as frequency divider, which can represent the values from 0 to 65535
/*
  Program the next event in oneshot mode

  Delta is given in PIT ticks
static int pit_next_event(unsigned long delta, struct clock_event_device *evt)
{
  spin_lock(&i8253_lock);
  outb_p(delta & 0xff , PIT_CH0); // LSB
  outb(delta >> 8 , PIT_CH0); // MSB
  spin_unlock(&i8253_lock);

  return 0;
}*/

/*   init_pit
*    initializes the PIT to make it send interrupts periodically
*    I/O: N/A
*    Return: N/A
*    Effects: initializes the PIT
*/
void init_pit(){//call this in kernel.c
  // initialize PIT channel 0 for any (possible) frequency and use IRQ 0 to accurately keep track of real time in milliseconds
  // Typically, OSes and BIOSes use mode 3 (see below) for PIT channel 0 to generate IRQ 0 timer ticks,
  // but some use mode 2 instead, to gain frequency accuracy (frequency = 1193182 / reload_value Hz).
  // Set freq : 65535-minimum
  // mode2 is not suitable for display sound ?
  int32_t lowbit_freq,highbit_freq;
  lowbit_freq = PIT_HZ/SET_HZ & PIT_MASK;
  highbit_freq = PIT_HZ/SET_HZ >> PIT_SHIFT;
  outb(MODE2, CMD_REG);
  outb(lowbit_freq, CHANEL_0);
  outb(highbit_freq, CHANEL_0);
  printf("init_pit");
  enable_irq(PIT_IRQ);

}



