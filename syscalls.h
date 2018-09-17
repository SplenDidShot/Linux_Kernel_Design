#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "lib.h"
#include "types.h"
#include "filesystem.h"
#include "dev_term.h"
#include "paging.h"
#include "x86_desc.h"
#include "rtc.h"
#include "dev_kbd.h"
#include "schedule.h"
typedef int32_t (*dummy_fn)(void);
typedef int32_t (*read_op)(int32_t fd, void* buf, int32_t nbytes);
typedef int32_t (*write_op)(int32_t fd, const void* buf, int32_t nbytes);
typedef int32_t (*open_op)(const uint8_t* filename);
typedef int32_t (*close_op)(int32_t fd);

//file array:
//1.The file operations jump table associated with the correct file type.
//2.The inode number for this file.
//3.A “file position” member that keeps track of where the user is currently reading from in the file.
//4. A “flags” member for, among other things, marking this file descriptor as “in-use.”
typedef struct file_descriptor{
  dummy_fn* fop_table;
  int32_t inode_type;
  int32_t offset;
  int32_t in_use_flag;
}file_des_t;

typedef struct pcb{//locate key information about a process.
    file_des_t file_array[8];// 0-stdin, 1-stdout,2-7 dynamic allocated file desc
    volatile uint32_t status;//  The current state of the process i.e., whether it is ready, running, waiting, or whatever.
    uint8_t arg[1024]; //max 128 chars in the arg field
    uint32_t pid; //The unique identification for each process
    struct pcb* parent_process_ptr;            //A pointer to parent process.
    struct pcb* child_process_ptr;             // a pointer to child process (if it exists).
    uint32_t * process_addr;                    //Pointers to locate memory of processes.
    uint32_t esp;  //parent esp;
    uint32_t ebp;  //parent esp;
	  uint32_t cr3;  //parent cr3;
    int32_t program_counter;
}pcb_t;

//returns the pcb for the latest process from the top of the 8MB memory area
pcb_t* get_pcb();
int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
int32_t set_handler (int32_t signum,void* handler_address);
int32_t sigreturn (void);
int32_t default_(void);


#endif
