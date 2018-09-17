#include "types.h"
#include "paging.h"

uint32_t page_directory[NUM_ENTRY] __attribute__((aligned(PAGE_SIZE)));
uint32_t page_table[NUM_ENTRY] __attribute__((aligned(PAGE_SIZE)));
uint32_t vid_page_table[NUM_ENTRY] __attribute__((aligned(PAGE_SIZE)));
// add more tests here
/* paging_setup
 * 
 * Initialize page 
 * Inputs: None
 * Outputs:None
 * Side Effects: None
 * Coverage: Init paging 
 * Files: paging.c /paging.h
 */
void paging_setup() {
	uint32_t i;
	/*Initialize page directory*/
  /*
  In the page directory, each entry points to a page table.
  */
  /*create a blank page directory, static */

	for (i = 0; i < NUM_ENTRY; i++) {
		// set up supervisor bit
      // 2         1        0
      //U/S      R/W        P
      //set any other unused pages to not present ,supervised,write
		page_table[i] = BLANK;
		page_directory[i] = BLANK;
	}
	page_table[VMEM >> PAGE_SHIFT] = VMEM | RW_P_MASK;
	
	page_directory[(unsigned int)page_table >> TAB_SHIFT] = (unsigned int)page_table | RW_P_MASK;
	//first page store addr of page_table
	page_directory[0]=((uint32_t)page_table) | RW_P_MASK;
	//map virtual memo 4-8MB to physical memo 4-8MB
	page_directory[1]= KERNEL_PHYS_VIRT_MEMO_ADDR |RW_P_MASK|FOUR_MB_PAGE;  //????????
	/* enable paging*/
   // 1. put PD  address into  cr3 register.
   // 2. set page 0 point to 4KB.
   // 3. load addr to  cr3 register
   // 3. PAE (bit 5 of cr4) used only after page enabled
   // 4. set the paging bit (32th) in cr0
	asm volatile (                            
     // "  orl (0x1000),%%eax;"
      "  movl %%eax, %%cr3  ;  "                                         
      "  movl %%cr4, %%eax;"
      "  orl $0x00000010,%%eax;"
      "  movl %%eax, %%cr4;"
      "  movl %%cr0, %%eax   ; "                                          
      "  orl $0x80000000, %%eax ;  "                                     
      "  movl %%eax, %%cr0  ; "                                           
        :
        :"A"((uint32_t)page_directory)
        : "eax","cc"
    );


}
/* set_up_program_paging
 * set up new page for the new process.
 * Inputs: int32_t process_number
 * Outputs:None
 * Side Effects: activate new page for new process. 
 * Files: paging.c /paging.h
 */
void set_up_program_paging(int32_t process_number) { //0 or 1
	/* 8MB for the first program, 12MB for the second program, etc */
	int32_t start_addr = (process_number)*FOUR_MB;  //8MB + 0/1*4MB   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	page_directory[PROGRAM_IDX] = (KERNEL_PHYS_VIRT_MEMO_ADDR + start_addr) |PROGRAM_PAGE_MASK;

	//Flush TLB
	asm volatile (
			"  movl %%cr3, %%eax  ;  "
			"  movl %%eax, %%cr3;"
				:
				:
				: "eax"
	);
	// asm volatile (                            
 //     // "  orl (0x1000),%%eax;"
 //      "  movl %%eax, %%cr3  ;  "                                         
 //      "  movl %%cr4, %%eax;"
 //      "  orl $0x00000010,%%eax;"
 //      "  movl %%eax, %%cr4;"
 //      "  movl %%cr0, %%eax   ; "                                          
 //      "  orl $0x80000000, %%eax ;  "                                     
 //      "  movl %%eax, %%cr0  ; "                                           
 //        :
 //        :"A"((uint32_t)page_directory)
 //        : "eax","cc"
 //    );
}

/* video mapping helper function
 * maps terminal program output to the correct virtual memory address.
 * Inputs: None
 * Outputs:None
 * Side Effects: enables programs such as pingpong and fish to create animations to the screen. 
 * Files: paging.c /paging.h
 */
uint8_t* vidmap_helper(){
	int32_t i ;
	uint32_t pt_idx;
	uint32_t pd_idx;

	/*add another 4kb page */
	for (i = 0; i < NUM_ENTRY; i++) {
		// set up supervisor bit
      // 2         1        0
      //U/S      R/W        P
      //set any other unused pages to not present ,supervised,write
		vid_page_table[i] = BLANK;
	}
	/*calc entry*/
	pd_idx = ((uint32_t) VID_MEM_INIT & PD_MASK);
	pt_idx = ((uint32_t) VID_MEM_INIT & PT_MASK);
	page_directory[pd_idx>>22] = (int)vid_page_table | VID_FOURKB;
	vid_page_table[pt_idx>>12] = VMEM | VID_FOURKB;

	/*flush TLB */
	asm volatile (
     // "  orl (0x1000),%%eax;"
      "  movl %%eax, %%cr3  ;  "
        :
        :"A"((uint32_t)page_directory)
        : "eax","cc"
    );
	return (uint8_t*)VID_MEM_INIT;
}


/*   map_term
*    swaps the next terminal that is to be switched on screen
*    Inputs: old_term - old terminal flag
*	     term_flag - new terminal to switched to
*	     addr - address of the target vid mem
*    Output: N/A
*    Return: N/A
*    Effects: switches screen to target terminal
*/
void map_term(uint8_t term_flag, uint8_t old_flag, int32_t* addr){
	vid_page_table[old_flag] = *addr | 0x7;
	vid_page_table[term_flag] = VMEM | 0x7;

	/*flush TLB */
	asm volatile (
     // "  orl (0x1000),%%eax;"
      "  movl %%eax, %%cr3  ;  "
        :
        :"A"((uint32_t)page_directory)
        : "eax","cc"
    );

}

/*   remap_terminal_schedule
*    swaps the context for the next terminal into the correct PD
*    Inputs: virtual_addr - virtual address that stores the target terminal video info
*	     phy_addr - physical address that vid mem is to be displayed
*    Output: N/A
*    Return: N/A
*    Effects: switches screen to target terminal on the PD level
*/
void remap_terminal_schedule(int32_t virtual_addr, int32_t phy_addr ){
	uint32_t pd_idx = virtual_addr / FOUR_MB;

//7 is 111 in binaries -> enable bits
page_directory[pd_idx] = ((int)vid_page_table) | 7;
vid_page_table[0] = phy_addr | 7; 
	/*flush TLB */
	asm volatile (
		 // "  orl (0x1000),%%eax;"
			"  movl %%eax, %%cr3  ;  "
				:
				:"A"((uint32_t)page_directory)
				: "eax","cc"
		);

}
