#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"

#include "syscalls.h"

#include "lib.h"
//magic number define here
#define FILENAME_LEN 32
#define BLOCK_SIZE 1023
#define FOUR_KB 0x1000
#define MAX_DENTRY_NUM 63
#define BB_RESERVED 52
#define DENTRY_RESERVED 24
#define REG_TYPE 2
#define PCB_BASE_ADDR 0x80000  //8MB
#define PCB_BLOCK			     0x2000 //8kb
#define MAX_FD  7
#define GREP_INODE 50
//struct defined here
/* boot block :
              holds both file system statistics and the directory entries.
*/

//index node that specifies the file’s size in bytes and the data blocks that make up the file.
typedef struct inode{
  int32_t length;
  int32_t data_block_num [BLOCK_SIZE];
} inode_t;

//dir_entry :32  name,file type,inode idx,
//File types are 0 for a file giving user-level access to the real-time clock (RTC), 1 for the directory, and 2 for a regular file.
//should be ignored for the RTC and directory types.
typedef struct dentry{
	uint8_t filename[FILENAME_LEN];//32
	uint32_t filetype;
	uint32_t inode;
	uint8_t  reserved[DENTRY_RESERVED];
} dentry_t;

typedef struct boot_block{
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[BB_RESERVED];
    dentry_t direntries[MAX_DENTRY_NUM];
} boot_block_t;


//Both the statistics and each directory entry occupy 64B


//function defined here
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
//only check that the given inode is within the valid range. It does not check that the inode actually corresponds to a file
//The last routine works much like the read system call, reading up to length bytes starting from position offset in the file with inode number inode and returning the number of bytes read and placed in the buffer. A return value of 0 thus indicates that the end of the file has been reached.
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern int32_t fs_init(int32_t start_addr);
extern void list_all_files();
int32_t fs_dir_open(const uint8_t* filename);
int32_t fs_dir_close(int32_t fd);
int32_t fs_dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t fs_dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t fs_file_open(const uint8_t* filename);
int32_t fs_file_close(int32_t fd);
int32_t fs_file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t fs_file_read(int32_t fd,void* buf ,int32_t nbytes);

#endif
