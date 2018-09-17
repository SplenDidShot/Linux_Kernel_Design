#include "filesystem.h"
#include "x86_desc.h"
//opening and reading from files, opening and reading the directory
//copying program images into contiguous physical memory from the randomly ordered 4 kB “disk” blocks that constitute their images in the file system.



//local variables
//dentry_t* dentries; 	//should not include the whole dentries???
static dentry_t dentry_global;
static boot_block_t boot_block;
int32_t inode_addr;
static int32_t boot_block_addr;
static int32_t data_block_addr;
volatile uint32_t dir_count;

/***********************************fs_init*********************************************/
/*
 * fs_init
 * DESCRIPTION: Init filesystem .Set the location of the boot block.
 *
 * I/O: starting address of boot_block
 * RETURN: 0 for done
 * SIDE EFFECTS: NONE.
 */
int32_t fs_init(int32_t start_addr){
  //copy boot_block info from kernel
  memcpy(&boot_block,(int32_t*)start_addr,FOUR_KB);
  //set address.
  boot_block_addr=start_addr;
  inode_addr =(int32_t)(start_addr + FOUR_KB);
  //data_block_addr =start_addr + (boot_block->inode_count +1) *FOUR_KB;
  data_block_addr = (int32_t)((inode_t*)inode_addr + (boot_block.inode_count));
  dir_count=0;
  return 0 ;
}

/***********************************  dir_ops *********************************************/
/*
 * fs_dir_open
 * DESCRIPTION: open all the entries in the directory.
 *
 * I/O: directory name
 * RETURN: 0 for done ,-1 for fail
 * SIDE EFFECTS: Store entries of directory in dentries.
 */
 //!!!!!!!!!!!!!!!!
int32_t fs_dir_open(const uint8_t* filename) {
  //first check whether the filename exist
  int32_t result;
  result = read_dentry_by_name(filename, &dentry_global);
  if(result == -1) return -1;

  return 0;

}
/*
 * fs_dir_close
 * DESCRIPTION:  Do nothing
 *
 * I/O: file descriptor
 * RETURN: 0 for done
 * SIDE EFFECTS: None
 */
int32_t fs_dir_close(int32_t fd){
  return close(fd);
}
/*
 * fs_dir_write
 * DESCRIPTION:  Do nothing
 *
 * I/O: file descriptor
 * RETURN: -1 for done
 * SIDE EFFECTS: None
 */
int32_t fs_dir_write(int32_t fd, const void* buf, int32_t nbytes){
  return -1;

}
/*
 * fs_dir_read
 * DESCRIPTION:  Store all the file name in the buffer
 *
 * I/O: file descriptor ,buf ,nbytes
 * RETURN: # of bytes in buffer
 * SIDE EFFECTS: None
 fd = dentries->inode,not used in cp2
 */
int32_t fs_dir_read(int32_t fd, void* buf, int32_t nbytes){
	/** need to double check this part*/
	//printf("dir_read dentry info\n");
	//printf("dir_read dentry_name is %s", dentry_global.filename);
	//printf("dir_read dentry_addr is %x", &dentry_global);
	int flag = 0;
	read_dentry_by_index(dir_count,&dentry_global);
	memset(buf, '\0', FILENAME_LEN);
	//printf("%d\n", strlen((int8_t*)dentry_global.filename));
	if(strlen((int8_t*)dentry_global.filename)>=FILENAME_LEN)
	{
		memcpy((uint8_t*)buf,(uint8_t*)dentry_global.filename,FILENAME_LEN);
		flag = 1;
	}
	else
		memcpy((uint8_t*)buf,(uint8_t*)dentry_global.filename,strlen((int8_t*)dentry_global.filename));

	dir_count++;
	if(dir_count> boot_block.dir_count) {
		dir_count=0;
		return 0;   
	}		//???????????
	return flag == 1 ? FILENAME_LEN : strlen((int8_t*)dentry_global.filename);
	
	//ls call read :  while (0 != (cnt = ece391_read (fd, buf, SBUFSIZE-1))) 
  // pcb_ version :
/*
  printf("enter dir_read\n");
  int32_t idx;
  pcb_t* pcb=get_pcb();
  idx = pcb->file_array[fd].offset;
  if( dir_count== boot_block.dir_count){
	  pcb->file_array[fd].offset=0;
	  dir_count =0;
	  return 0;
  }
  if(read_dentry_by_index(idx,&dentry_global)==-1) {
    return -1;
  }
  memcpy(buf,(uint8_t*)dentry_global.filename,32);
  pcb->file_array[fd].offset+=32;
  return 32;*/
}

/***************************************** file ops**********************************************************/

/*
 * fs_file_open
 * DESCRIPTION:Open
 *
 * I/O: file descriptor
 * RETURN: 0 for done
 * SIDE EFFECTS: None
 */
/* initialize any temporary structures, return 0*/
int32_t fs_file_open(const uint8_t* filename){
	//printf("filename : %s\n",filename);
	int32_t result = read_dentry_by_name(filename, &dentry_global);
	if(dentry_global.filetype != REG_TYPE) return -1;
//????????????????do we need to set the file desc flag to 1 ????
	if(result==-1) return -1;

  return 0;
}
/*
 * fs_file_close
 * DESCRIPTION: Close file
 *
 * I/O: file descriptor
 * RETURN: 0 for done
 * SIDE EFFECTS: None
 */
int32_t fs_file_close(int32_t fd){
  //pcb version :
  return close(fd);

}
/*
 * fs_file_write
 * DESCRIPTION: Do nothing
 *
 * I/O: file descriptor
 * RETURN: 0 for done
 * SIDE EFFECTS: None
 */

int32_t fs_file_write(int32_t fd, const void* buf, int32_t nbytes){
  return -1;
}


/*
 * fs_file_read
 * DESCRIPTION: Read file byte by byte
 *
 * I/O:uint8_t * buf,uint8_t* fname,uint32_t offset,uint32_t length
 * RETURN: # of byte in buffer .
 * SIDE EFFECTS: None
 */
int32_t fs_file_read(int32_t fd,void* buf ,int32_t nbytes){
/* 	int32_t result;
  if(dentry_global.inode <0  || dentry_global.inode > boot_block.inode_count) return 0; // initial file position is at or beyongd the EOF, 0 return ?????????????????????????????????????????
  result= read_data(dentry_global.inode,0,buf,nbytes);
  pcb_t* pcb = get_pcb();
  pcb->file_array[fd].offset+=result;
  return result; */
  // pcb_version:
	//printf("in file read\n");
  pcb_t* pcb = get_pcb();
 // printf("check read1\n");
  int32_t next_offset=read_data(pcb->file_array[fd].inode_type,pcb->file_array[fd].offset,buf,nbytes);
 //  printf("check read2\n");
  pcb->file_array[fd].offset+=next_offset;
  // printf("check read3\n");
  
 // printf("leav ret\n");
  return next_offset;
}


/***********************************************Three_read_helper_functions*************************************************************/

//return -1 on failure :indicating a non-existent file or invalid index in the case of the first two calls,
//or an invalid inode number in the case of the last routine.
//When successful, the first two calls fill in the dentry t block passed as their
//second argument with the file name, file type, and inode number for the file, then return 0.
/*
< Scans through the directory entries in the “boot block” to find the file name >
Call read_dentry_by_index() {
< Populates the dentry parameter -> file name, file type, inode number >*/


/*
 * read_dentry_by_name
 * DESCRIPTION: Read file byte by byte
 *
 * I/O:uint8_t * buf,uint8_t* fname,uint32_t offset,uint32_t length
 * RETURN: # of byte in buffer .
 * SIDE EFFECTS: None
 */

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
  uint32_t i; //inode index
  //check for invalid situation
  if(!fname || strlen((int8_t*)fname)==0) //!!!!!!!!!!!!!!!!!!!
    return -1; //fail
  //check inode num 25
	// printf("25th inode %s\n", (int8_t*)boot_block.direntries[25].filename);
	// printf("50th inode %s\n", (int8_t*)boot_block.direntries[50].filename);
  for (i=0;i<MAX_DENTRY_NUM;i++){ // magic #, MAX dentry #
	//tmp = strncmp((int8_t*)boot_block.direntries[i].filename, (int8_t*)fname, (uint32_t)FILENAME_LEN);
	//printf("curr_bb_dir_name %s\ncur fname %s, are they equal %d",boot_block->direntries[i].filename, fname,tmp);
    if(strncmp((int8_t*)boot_block.direntries[i].filename, (int8_t*)fname, FILENAME_LEN)==0)
    {
    
      if(read_dentry_by_index(i, dentry) == -1)
      	return -1;
      else
	    return 0;
	}
  }
  return -1; // not found
}

/*
 * read_dentry_by_name
 * DESCRIPTION: Read file byte by byte
 *
 * I/O:uint8_t * buf,uint8_t* fname,uint32_t offset,uint32_t length
 * RETURN: # of byte in buffer .
 * SIDE EFFECTS: None
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){

  if(index>MAX_DENTRY_NUM || index < 0 || !dentry ) return -1 ;// idx out of range

   *dentry = boot_block.direntries[index];
  // printf("page fault check\n");
  // printf("The name of current entry is %s filetype is %d \n", dentry->filename, dentry->filetype);
   // strcpy((int8_t*)dentry->filename, (int8_t*)boot_block->direntries[index].filename);
   // dentry->filetype= boot_block->direntries[index].filetype;
   // dentry->inode = boot_block->direntries[index].inode;
 //  printf("\n the entry address is %x", dentry);
   return 0;
}

/*like the read system call, reading up to length bytes starting from position offset
in the file with inode number inode
 and returning the number of bytes read and placed in the buffer.
A return value of 0 thus indicates that the end of the file has been reached.
*/

/*
 * read_data
 * DESCRIPTION: Read file byte by byte
 *
 * I/O:uint8_t * buf,uint8_t* fname,uint32_t offset,uint32_t length
 * RETURN: # of byte in buffer .
 * SIDE EFFECTS: None
 */

// second call offset = filelength , return 0 indicate EOF.
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
	//printf("in read data\n");
	inode_t inode_local;	//alias for opened inode
	int32_t read_byte;		//byte already read
	int32_t i;				//idx for block number
	uint8_t* curr_block_addr;		//curr block's addr
	int32_t left_length;
	int32_t else_byte;
	int32_t in_blk_pos;

	//inode_local = ((inode_t*)inode_addr)[inode];
	//inode_local = *((inode_t*)inode_addr + inode);
	inode_local = *((inode_t*)(inode_addr + inode * FOUR_KB));
	//printf("inode_length %d \n", inodes[inode].length);

	/* check whether length valid */

	if(length==0  || buf == NULL|| offset>inode_local.length) return -1;
	/* var init */
	read_byte = 0;

	//blk_nums = inode_local->length / FOUR_KB;
	in_blk_pos = offset%FOUR_KB;
	//check EOF vs length
	left_length = length+ offset > inode_local.length ? inode_local.length - offset :length;
	/* start read data from data block */

	while (read_byte <= left_length) {
		//if( read_byte == inode_local.length) return 0; //reach EOF
		i = (read_byte + offset) / FOUR_KB; //idx of blk
		if( inode_local.data_block_num[i] <0 || inode_local.data_block_num[i]>=boot_block.data_count) return -1; //bad block #
		curr_block_addr = (uint8_t*)(data_block_addr + inode_local.data_block_num[i] * FOUR_KB);

		in_blk_pos = (read_byte+offset)%FOUR_KB;
		//check whether to-be-read data is inside the curr block

		//all the left byte need to copy is inside curr blk
		if (left_length - read_byte + in_blk_pos< FOUR_KB) {
			memcpy(buf + read_byte, curr_block_addr + in_blk_pos, left_length - read_byte);
			read_byte = left_length;
			break;
		}
		else {
			//left byte is more than curr_blk size , copy entire blk
			else_byte = (FOUR_KB-in_blk_pos)>(left_length-read_byte) ? (left_length-read_byte):(FOUR_KB-in_blk_pos);
			memcpy(buf + read_byte, curr_block_addr , else_byte);
			//read_byte+=FOUR_KB-in_blk_pos;
			read_byte+=else_byte;

		}

	}
	//printf("lead data buf : %s \n",buf );
	return read_byte;
}


/******************************************************TEST_FOR_FILESYS**************************************************/
