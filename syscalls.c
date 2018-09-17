#include "syscalls.h"
#include "paging.h"
#define     _1KB              (1 << 10)
#define     _8KB              (8 << 10)
#define     _1MB              (1 << 20)
#define     _4MB              (4 << 20)
#define     _8MB              (8 << 20)
#define 	_128M			  (128 << 20)
#define     EXE_FSIZE 	      _4MB
#define     EXE_PROBE_4B 	    4
#define     EXE_MAGIC_BYTE0	  0x7f
#define     EXE_MAGIC_BYTE1	  0x45
#define     EXE_MAGIC_BYTE2	  0x4c
#define     EXE_MAGIC_BYTE3	  0x46
#define     EXE_MEMV_BASE     0x08048000	
#define 	EXE_MEMP_BASE	  _8MB
#define     EXE_ENTRY_OFF     24
#define     KSTACK_SIZE       _8KB  
#define     KSTACK_BASE       _8MB - _8KB
#define     USTACK_BASE       ((132 << 20) -4)
/* to use the following macro function, the first process id must be 0 */
#define     KSTACK_TO_PCB(x)  (KSTACK_BASE - ((x) + 1) * KSTACK_SIZE)
#define     RUNNING           1
#define     IDLE              0
#define     IF_MASK           0x200
#define     FD_START_FILE     2
#define     FD_MAX_FILE       8
#define     STDIN_FILE      0
#define     STDOUT_FILE    1   
#define 	PCB_ADDR_MASK 0xFFFFE000
static int32_t active_proc = -1; //this is wrong, but it's ok!!!
static int8_t process_counter = 0;
extern term_t term_arr[3];
extern uint8_t curr_exec_term;
extern uint8_t term_flag;
//4 kinds of operations
dummy_fn file_table[4] = {(dummy_fn)fs_file_read, (dummy_fn)fs_file_write, (dummy_fn)fs_file_open, (dummy_fn)fs_file_close};
dummy_fn dir_table[4] = {(dummy_fn)fs_dir_read, (dummy_fn)fs_dir_write, (dummy_fn)fs_dir_open, (dummy_fn)fs_dir_close};
dummy_fn rtc_table[4] = {(dummy_fn)rtc_read, (dummy_fn)rtc_write, (dummy_fn)rtc_open, (dummy_fn)rtc_close};
dummy_fn stdin_table[4] = {(dummy_fn)term_read, (dummy_fn)default_, (dummy_fn)default_, (dummy_fn)default_};
dummy_fn stdout_table[4] = {(dummy_fn)default_, (dummy_fn)term_write, (dummy_fn)default_, (dummy_fn)default_};

/*	get_pcb
*	returns top address of kernel stack to get latest pcb addr
*	I/O: N/A
*	Return: pcb_addr
*	Side Effects: N/A
*/
pcb_t* get_pcb(){
	uint32_t pcb_addr;
	asm volatile(
		"movl	%%esp, %0"	
		:"=r" (pcb_addr)
		:
		:"memory"
	);
	pcb_addr &= PCB_ADDR_MASK;
	return (pcb_t*) pcb_addr;
}

/*
 * systemcall: halt
 * DESCRIPTION:Terminate a process 
 * I/O: uint8_t status
 * RETURN: 0 for success, 1 for fail 
 * SIDE EFFECTS: switch from user-level to kernel level  
 */
int32_t halt(uint8_t status){
	//printf("enter halt\n");
	int32_t i;
	int32_t parent_status;
	pcb_t* pcb = get_pcb();
	uint8_t* cmd_string = (uint8_t*)"shell";
	//int32_t pcb_idx = get_active_pcb_idx();
	// pcb =(pcb_t*)(PCB_BASE_ADDR - PCB_BLOCK*pcb_idx); //get the address of current process 
	parent_status =(int32_t)status;
	//if close the old shell, should open a new shell
	active_proc--;
	process_counter--;
	
	//printf("curr_term:%d , curr_num_program : %d " ,term_flag,term_arr[term_flag].num_program);
	if(pcb->parent_process_ptr==NULL ||active_proc ==-1||term_arr[term_flag].num_program<=1) { //??????????????????????????????????????????????????? If the halt shell,
		//printf("enter reexec shell\n");
		term_arr[term_flag].num_program--;
		execute(cmd_string);
	}
	term_arr[term_flag].num_program--;
	//update tss 
	
	//tss.esp0= pcb->esp;
  tss.esp0 = KSTACK_BASE - KSTACK_SIZE * (active_proc)-4;
	/*Restore parent data*/
	
	
	/*Restore parent paging*/
	
		
 set_up_program_paging(active_proc); //this is missong before !!!!

	//restore cr3
	
	//               halt cr3 not correct. 
	//printf("halt proc_num : %d\n",active_proc);
	//asm volatile (
	//		"  movl %%cr3, %%eax  ;  "
	//		"  movl %%eax, %%cr3;"
	//			:
	//			:
	//			: "eax"
	//);
	

	/*Close any relevant FDs*/
	for(i=FD_START_FILE;i<FD_MAX_FILE;i++){
		if(pcb->file_array[i].in_use_flag ==1){
			//printf("enter close\n");
			close(i); 
			//printf("leave close\n");
		}
	}
		
// set_up_program_paging(active_proc);
	/*return the specified value to its parent process.*/
	//asm volatile (                            
  //  
  //    "  movl %%eax, %%esp  ;  "                                                                      
  //      :
  //      :"A"((uint32_t)pcb->esp)
  //      : "eax","esp"
  //  );
	//printf("1st asm \n");
	//asm volatile (                            
  //  
  //    "  movl %%eax, %%ebp  ;  "                                                                         
  //      :
  //      :"A"((uint32_t)pcb->ebp)
  //      : "eax","ebp"
  //  ); 
	//printf("2 asm \n");
    	/*Jump to execute return*/
	//printf("pcb value is %x\n", pcb);
	//printf("ebp, esp value are %x %x\n", pcb->ebp, pcb->esp);
 // !!!!!!!!!!!!!!!!!!!!!!!!!somehow, separate restoring ebp, esp and jmp insruction does not work
	
	
	/*return extended 32 bit status to execute call*/
	asm volatile (                            
      "  pushl %2  ;  "     
			"movl %0, %%ebp;"
			"movl %1, %%esp;"
	  "  jmp EXEC_RET  ;  "	  
        :
        :"r"((uint32_t)pcb->ebp), "r"((uint32_t)pcb->esp), "r"((uint32_t)parent_status)
    );

	//printf("leave halt\n");
	return 0;

}
/*
 * systemcall: execute 
 * DESCRIPTION:Starta new process, create corresponding pcb 
 * I/O: const uint8_t* command
 * RETURN: 0 for success, -1 for fail 
 * SIDE EFFECTS: switch from user-level to kernel level  
 */
int32_t execute(const uint8_t* command){
	if((term_arr[0].num_program +term_arr[1].num_program+term_arr[2].num_program)>=6)
	{
		printf("Exceeded maximum program count, operation denied\n");
		return -1;
	}
	/* Parse args: first word is file name and rest are args for the program */
	if(!command)
		return -1;
	cli();
	// for cp3, ignore args first
	int i;
	//pcb_t* parent_pcb;
	uint8_t exe_fname[KBD_BUF_SIZE];
	//parsing terminal command
	for (i = 0; i < KBD_BUF_SIZE; i++) {
		if (command[i] == '\n' || command[i] == ' ' || command[i] == '\0')
			break;
		exe_fname[i] = command[i];
	}
	//set end of filename to 0
	exe_fname[i] = '\0';
	i++;
	uint8_t exe_args[KBD_BUF_SIZE] = "";
	int j = 0;
	//parsing command arguments
	if (active_proc != -1) {// not the first process, must attempt to extract args
	  	for (; i < KBD_BUF_SIZE; i++) {
			if (command[i] == '\n' || command[i] == ' ' || command[i] == '\0')
	        break;
	    	exe_args[j++] = command[i]; /// j++ or ++j 
	    }
	}
	exe_args[j] = '\0';
	// if no args for cat or grep, return -1
	// if((strncmp((int8_t*)exe_fname, "cat ", strlen((int8_t*)exe_fname))==0 || 
	// 	strncmp((int8_t*)exe_fname, "grep", strlen((int8_t*)exe_fname))==0) && 
	// 	strlen((int8_t*)exe_args)==0)
	// {
	// 	printf("need argument input\n");
	// 	return -1;
	// }
  	//printf("%s\n", exe_args);
	/* executable check */
	uint8_t exe_probe_buf[EXE_PROBE_4B];
	dentry_t loc_dentry;
	//printf("%s\n", exe_fname);
	
	if(read_dentry_by_name(exe_fname,&loc_dentry)==-1) return -1; // to debug easily, do not nest system call here.
	//printf("file open?\n");
	if (read_data(loc_dentry.inode, 0, exe_probe_buf, EXE_PROBE_4B) == -1) return -1;
	

	//printf("%d\n", loc_dentry.inode);
	//printf("pause 2\n");
	if (exe_probe_buf[0] != EXE_MAGIC_BYTE0 ||
			exe_probe_buf[1] != EXE_MAGIC_BYTE1 ||
			exe_probe_buf[2] != EXE_MAGIC_BYTE2 ||
			exe_probe_buf[3] != EXE_MAGIC_BYTE3) 
		return -1;
    active_proc++;
    process_counter++;
    term_arr[term_flag].num_program++;
	//if (read_data(loc_dentry.inode, 0, exe_probe_buf, 4) == -1) return -1;
	// there maybe some problems with offset args
	//offset 23 and 24 and read four bytes are c0 95 04 08

	//this block tests the stack values (endianness)
 	// printf("the data is from the original file\n");
	// for(i=0; i<1; i++)
	// {	
	// 	printf("%x\n", exe_probe_buf[i]);
	// }

	/* setup paging */
    set_up_program_paging(active_proc);//!!!!!!!!!!!!!!!arg
    //printf("page setup is succesfully, access 128M mem addr: %x\n addr is %ld", *((uint8_t*)EXE_MEMV_BASE),EXE_MEMV_BASE);
	/* load the program */
	
	if (read_data(loc_dentry.inode, 0,(uint8_t*)EXE_MEMV_BASE, EXE_FSIZE) == -1) return -1;

	
  //uint32_t exe_entry;
  //use uint8_t* pointer to find the 24-27 byte offset position first,
  //the cast into uint32_t* to read the entry point
  //exe_entry = *((uint32_t*)(((uint8_t*)EXE_MEMV_BASE) + EXE_ENTRY_OFF));

  /* create PCB */
  pcb_t* child_pcb = (pcb_t*)(KSTACK_TO_PCB(active_proc));
  //printf("after create child pcb\n");
  // initialize new pcb field
  child_pcb->status = RUNNING;
  
  /* need some help here to initialize filedescriptor */
  child_pcb->pid = active_proc ;
  //printf("curr_exec_term is %d ",term_flag);
  if (term_arr[term_flag].term_pcb ==NULL) {
  	child_pcb->parent_process_ptr = NULL;
  	term_arr[term_flag].term_pcb  =child_pcb;
  }
  else {
      child_pcb->parent_process_ptr = (pcb_t*)term_arr[term_flag].term_pcb;
      term_arr[term_flag].term_pcb = child_pcb;

      //parent_pcb = (pcb_t*)(KSTACK_TO_PCB(active_proc-1));
      //parent_pcb->status = 0; //sleep the parent process.
  }
  /*save command args into pcb*/
  memset(child_pcb->arg, '\0', _1KB);
  memcpy(child_pcb->arg, exe_args, strlen((int8_t*)exe_args));
  //printf("%s\n", child_pcb->arg);

  /*  init pcb fd_array*/
  //printf("start init fd \n");
  //stdin:
  child_pcb->file_array[STDIN_FILE].fop_table = stdin_table;
  child_pcb->file_array[STDIN_FILE].inode_type = NULL;
  child_pcb->file_array[STDIN_FILE].offset=0;
  child_pcb->file_array[STDIN_FILE].in_use_flag =1;
  //stdout:
  child_pcb->file_array[STDOUT_FILE].fop_table = stdout_table;
  child_pcb->file_array[STDOUT_FILE].inode_type = NULL;
  child_pcb->file_array[STDOUT_FILE].offset=0;
  child_pcb->file_array[STDOUT_FILE].in_use_flag =1;
  //2-7:
  for(i=FD_START_FILE;i<FD_MAX_FILE;i++){
	child_pcb->file_array[i].fop_table= NULL;
	child_pcb->file_array[i].inode_type = NULL;
	child_pcb->file_array[i].offset=0;
	child_pcb->file_array[i].in_use_flag =0;
  }
  //printf("after init fd \n");
  asm volatile (
      "movl %%esp, %0;"
      "movl %%ebp, %1;"
       :"=r"(child_pcb->esp), "=r"(child_pcb->ebp)
      ); 
	//printf("the value of current ebp and esp %x, %x\n", child_pcb->ebp, child_pcb->esp);
  // program counter field?????
  // ref: http://www.jamesmolloy.co.uk/tutorial_html/index.html
  /* create stack for iret:
   * stack should look like 
   * 
   * ------- <- stack top
   * eip: exe_entry 
   * -------
   * cs: user code seg
   * -------
   * eflags
   * ------
   * esp: bottom of program img page 
   * ------
   * ss: user ds 
   * ------*/
 
  tss.esp0 = KSTACK_BASE - KSTACK_SIZE * (active_proc)-4;
  int32_t status;
  uint32_t ustack_base = USTACK_BASE;
  uint32_t exe_memv_base = EXE_MEMV_BASE;
  uint16_t user_ds = USER_DS;
  //printf("before iret \n");
  //clear_screen();
  asm volatile (
     // "cli;"
/*   	  "movw  %3, %%ds;"
  	  "movw %3, %%es;"
  	  "movw %3, %%fs;"
  	  "movw %3,%%gs;" */
      "pushl $0x002B;"		/* USER_DS = 0x002B */
 
      "pushl %1;" /* USTACK_BASE       (_1MB << (128 + 4)) */
      "pushf  ;"
      "popl   %%eax;"
      "orl    $0x200,%%eax ;" /* IF_MASK = 0x200 */
      "pushl %%eax;"
      "pushl  $0x0023;" /*USER_CS     0x0023 */
      "movl %2, %%eax;"
      "pushl  24(%%eax);" /* EXE_MEMV_OFF(EXE_MEMV_BASE) */
      "iret;"
      "EXEC_RET:;"
			//"popl %%eax;"
      "movl %%eax,%0;"
      :"=r"(status)
      :"r"(ustack_base), "r"(exe_memv_base), "r"(user_ds)
      :"eax"
      );
 // 	printf("ustack_base: %x\n", status);
	return status;

}

/*
 * systemcall: read
 * DESCRIPTION:Read call for files 
 *
 * I/O: int32_t fd, void* buf, int32_t nbytes
 * RETURN: # of bytes read 
 * SIDE EFFECTS: 
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	//printf("enter read\n");
	//returns -1 upon invalid args
	//cannot read from stdout (fd = 1)
	if(nbytes<0 || fd<0 || fd>MAX_FD || fd == STDOUT_FILE || !buf)
		return -1;
	pcb_t* loc_pcb = get_pcb();
	//read from unopened fd returns -1
	if(loc_pcb->file_array[fd].in_use_flag == 0 )
		return -1;
	// 0 for read operations
	//printf("leave read\n");
	return ((read_op)(loc_pcb->file_array[fd].fop_table[0]))(fd, (char*) buf, nbytes);
}
/*
 * systemcall: write 
 * DESCRIPTION:write call for files 
 *
 * I/O: int32_t fd, const void* buf, int32_t nbytes
 * RETURN: # of bytes written.
 * SIDE EFFECTS: 
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
	//printf("enter write\n");
	//returns -1 upon failure
	//cannot write to stdin (fd = 0)
	if(nbytes<0 || fd<STDOUT_FILE || fd>MAX_FD)
		return -1;
	pcb_t* loc_pcb = get_pcb();
	if(loc_pcb->file_array[fd].in_use_flag == 0 )
		return -1;
	// 1 for write operations
	//printf("leave write\n");
	return ((write_op)(loc_pcb->file_array[fd].fop_table[1]))(fd, (char*) buf, nbytes);
}

/*
 * systemcall: open
 * DESCRIPTION:Provide access to the file system.
 *
 * I/O: const uint8_t* filename
 * RETURN: -1 for failure, fd for file descriptor idx.
 * SIDE EFFECTS: 
 */

int32_t open(const uint8_t* filename){
	/*local variables*/
	//printf("%s\n", filename);
	//printf("enter open\n");
	/*check valid input */
	if(!filename || strlen((int8_t*)filename)==0) return -1;
	int32_t i;
	int32_t fd;
	uint32_t type;
	dentry_t loc_dentry;
	pcb_t* loc_pcb = get_pcb();

	/*check if the filename exist*/
	//printf("open check file exist\n");
	/* printf("in open input filename is : %s \n",filename);
	printf("in open input filename finish\n"); */
	if(read_dentry_by_name(filename,&loc_dentry)==-1) return -1;
	//printf("after open check file exist\n");
	/*allocate unused file descriptor*/
	fd=0;
	for(i=FD_START_FILE;i<FD_MAX_FILE;i++){
		if(loc_pcb->file_array[i].in_use_flag==0){
			//printf("found fd %d\n",i);
			fd=i;
			break;
		}
	}
	/*file_array full, can not open more */
	if(fd==0){
		//printf("fd=0 exit \n"); 
		return -1;
	}
	

	/*set up necessary data to different type of files*/
	loc_pcb->program_counter++;
	type = loc_dentry.filetype;
	//printf("type :%d\n",type);
	switch(type){
		case 0://rtc file
			loc_pcb->file_array[fd].fop_table = rtc_table;
			loc_pcb->file_array[fd].inode_type  = NULL;
			loc_pcb->file_array[fd].offset = 0;
			loc_pcb->file_array[fd].in_use_flag =1;
			break;
		case 1://directory 
			//printf("dir \n");
			loc_pcb->file_array[fd].fop_table = dir_table;
			loc_pcb->file_array[fd].inode_type  = NULL;
			loc_pcb->file_array[fd].offset = 0;
			loc_pcb->file_array[fd].in_use_flag =1;
			break;
		case 2://reg file
			loc_pcb->file_array[fd].fop_table = file_table;
			loc_pcb->file_array[fd].inode_type  = loc_dentry.inode;
			loc_pcb->file_array[fd].offset = 0;
			loc_pcb->file_array[fd].in_use_flag =1;
			break;
		default:;		
	}
	//printf("fd: %d\n", fd);
//	printf("leave open\n");
	return fd;
}

/*
 * systemcall: close
 * DESCRIPTION:Close the current file descriptor and make it available for later use.
 * I/O: const uint8_t* filename
 * RETURN: 0 for success, 1 for fail 
 * SIDE EFFECTS: 
 */
int32_t close(int32_t fd){
	/*check for default descriptors*/
	//printf("enter close\n");
	if(fd <2 || fd >MAX_FD) return -1;
	// int32_t pcb_idx = get_active_pcb_idx();
	// pcb_t* loc_pcb = (pcb_t*)(PCB_BASE_ADDR-PCB_BLOCK*pcb_idx);
	pcb_t* loc_pcb = get_pcb();
	if(loc_pcb->file_array[fd].in_use_flag!=1) return -1;
	loc_pcb->file_array[fd].in_use_flag = 0;
	loc_pcb->file_array[fd].inode_type=NULL;
	loc_pcb->file_array[fd].fop_table=NULL;
	loc_pcb->file_array[fd].offset =NULL;
	loc_pcb->program_counter--;
	//printf("leave close\n");
	return 0;
}

/*
 * systemcall: getargs
 * DESCRIPTION: Allows systemcalls that require argument inputs to work properly
 * I/O:  uint8_t* buf - buffer to store args, nbytes - number of bytes to be copied
 * RETURN: 0 for success, 1 for fail 
 * SIDE EFFECTS: 
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){
		/*check input*/
	//printf("in\n");
	if(buf == NULL || nbytes < 0 )
		return -1;

	/* (no arguments) || (the arguments and a terminal NULL (0-byte) do not fit in the buffer)*/
	pcb_t* loc_pcb = get_pcb();
	//printf("pcb arg :%s\n", loc_pcb->arg);
	if(!loc_pcb || nbytes < (strlen((int8_t *)loc_pcb->arg)+1) || strlen((int8_t *)loc_pcb->arg)==0)
		return -1;
	memset(buf, '\0',_1KB);
	memcpy(buf, loc_pcb->arg, strlen((int8_t*)loc_pcb->arg));

	return 0;

}


	 /*
 * systemcall: vidmap
 * DESCRIPTION:maps the text-mode video memory into user space at a pre-set virtual address.
 * I/O: const uint8_t* filename
 * RETURN: 0 for success, -1 for fail
 * SIDE EFFECTS:
 */
int32_t vidmap(uint8_t** screen_start){
	if(screen_start==NULL || (int32_t)screen_start <0x8000000 || (int32_t)screen_start >0x8400000 ) return -1;
	else {
		*screen_start =(uint8_t*) vidmap_helper(); 
		return 0;
	}
}
int32_t sigreturn (void){
	return -1;
}
int32_t set_handler (int32_t signum,void* handler_address){
	return -1;
}

int32_t default_(void) 
{
	return -1;
}

