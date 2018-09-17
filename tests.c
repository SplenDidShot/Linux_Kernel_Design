#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "dev_kbd.h"
#include "dev_term.h"
#include "rtc.h"
#include "filesystem.h"
#include "vga.h"
#define PASS 1
#define FAIL 0
#define VERY_LARGE_INT 500000000
#define LARGE_INT	9999
#define	FNAME_SIZE	32
#define KBD_BUF_SIZE	128

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here
/* Paging  Test - Example
 * 
 * check the initialization method
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Init paging 
 * Files: paging.c /paging.h
 */
int paging_test(){
	uint32_t* invalid_addr;
	uint32_t* valid_addr;
	uint32_t  page_fault_test;
	uint32_t i = 0xB8000;
	
	invalid_addr= NULL;
	valid_addr = &i;
	page_fault_test= *(valid_addr);
	printf("Test %d \n",valid_addr);
	page_fault_test= *(invalid_addr);//should show page fault on screen.
	printf("Test %d",invalid_addr);

	
	return 0;
}
/*
* dev by zero
* DESC: the testcase that divides an arbitrary number by zero to invoke exception
* I/O: N/A
* RETURN: N/A
* SIDE EFFECTS: invokes the exception and halts the system
*/
int dev_by_zero(){
	uint32_t i = 5;
	uint32_t j = 0;
	return i/j;
}

/* Checkpoint 2 tests */

/*	terminal test - terminal driver test
*	I/O: N/A
*	Return: 1 on success, 0 on failure
*	Side Effects: echoes the kbd inputs twice if terminal driver functions are correct
*/
int terminal_test(){
	uint8_t buf[KBD_BUF_SIZE];
	uint32_t i, j;
	if((i=term_read(0, buf, KBD_BUF_SIZE)) > 0)
		j=term_write(0, buf, KBD_BUF_SIZE);
	printf("read buf:%d, wrtie buf:%d\n", i,j);
	return i==j;
}

/*	rtc test - rtc driver test
*	I/O: N/A
*	Return: 1 on success, 0 on failure
*	Side Effects: prints characters on the terminal according to the rtc frequency
*/
// int rtc_test(){
// 	//frequency is 2 Hz in the beggining (16-1)shift right thingy = 15
// 	uint8_t freq = 15;
// 	uint32_t i = 0;
// 	while(freq>3)
// 	{
// 		rtc_write(0, (uint8_t*)(&freq));
// 		while(i++<VERY_LARGE_INT);
// 		i=0;
// 		freq--;
// 	}
// 	freq = 15;
// 	rtc_write(0, (uint8_t*)(&freq));
// 	return 1;
// }

/*	fs test - filesystem driver test
*	I/O: N/A
*	Return: 1 on success, 0 on failure
*	Side Effects: prints the output content of the filesystem read functions
*/
int fs_test(){
	uint8_t buf[FNAME_SIZE];
	uint8_t buff[LARGE_INT];
	int32_t i;
	int32_t result;
	i=0;
	//first check dir_read 
	printf("DIR READ TEST \n");
	//fs_dir_open((uint8_t*)".");
	while(fs_dir_read(0, buf, FNAME_SIZE)!=0){
		printf("%s\n",buf);
	}

	//then check file read 
/* 	clear_screen();
	printf("FILE READ TEST\n");
	
	printf("TEST VERY LARGE READ FILE \n");
	result = fs_file_open((const uint8_t*)"verylargetextwithverylongname.tx");
	if(result ==0){
		fs_file_read(0,buff,LARGE_INT);
		printf("%s\n",buff);
	}
	else {
		printf("\nNON TEXT FILE \n");
	} */
	clear_screen();
	printf("TEST NON TEXT FILE  \n");
	result = fs_file_open((const uint8_t*)"pingpong");
	//if(result == 0){
		memset(buff, '\0', LARGE_INT);
		fs_file_read(0,buff,LARGE_INT);
		for (i=0; i<LARGE_INT; i++)
		{
			if(buff[i]!='\0')
			putc(buff[i]);
		}
		putc('\n');
		set_cursor_loc((uint16_t)get_screen_loc());
		
	/* else {
		printf("NON TEXT FILE \n");
		fs_file_read(0,buff,LARGE_INT);
		for (i=0; i<1500; i++)
			putc(buff[i]);
	} */
	/* clear_screen();
	printf("TEST NORMAL TEXT FILE   \n");
	result = fs_file_open((const uint8_t*)"frame1.txt");
	if(result ==0){
		fs_file_read(0,buff,LARGE_INT);
		printf("%s\n",buff);
	}
	else {
		printf("\nNON TEXT FILE \n");
	}  */
	return 1;	
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//TEST_OUTPUT("itr_test", test_interrupts());
	//TEST_OUTPUT("paging_test", paging_test());
	//TEST_OUTPUT("dev by 0 test", dev_by_zero());

	TEST_OUTPUT("term_test", terminal_test());
	//TEST_OUTPUT("rtc_test", rtc_test());
	//TEST_OUTPUT("fs_test",fs_test());
	set_cursor_loc((uint16_t)get_screen_loc());
}
