#include "dev_term.h"

extern uint8_t curr_exec_term;

//uint8_t* term_local;

/*	term_open - opens the terminal ... ?
*	I/O: N/A
*	Return: 0
*	Side Effects: N/A (empty function)
*/
int32_t term_open(const uint8_t* filename)
{
	return 0;
}

/*	term_close - closes the terminal ... ?
*	I/O: N/A
*	Return: 0
*	Side Effects: N/A (empty function)
*/
int32_t term_close(int32_t fd)
{

	return 0;
}

/*	term_read - reads string from the kbd buffer for commands
*	Inputs : buf - buffer to store the returned string
*			 nbytes - number of bytes to read (fixed to 128 really)
*	Return: nbytes - number of bytes read
*	Side Effects: delivers the user input to shell
*/
int32_t term_read(int32_t fd, void* buf, int32_t nbytes)
{

	if (buf == NULL || nbytes < 0)
		return -1;

	while(!is_kbd_ready()) ; //keep waiting until data is available
	unset_kbd_ready();

	uint8_t* term_local = get_kbd_buf();
	//printf("%s", term_local);
	memset((uint8_t*)buf, '\0', KBD_BUF_SIZE);
	//memcpy((uint8_t*)buf, term_local, count);
	int32_t i = 0;
	while (term_local[i] != '\n' ) {
		*((uint8_t*)buf+i) = term_local[i];
		i++;
	}
	//printf("print buffer in terminal read%s, i = %d\n", (uint8_t*)buf, i);

	return i;
}

/*	term_write - writes string from the terminal buffer to the terminal
*	Inputs : buf - buffer that stores the target string
*	Return: i - number of bytes written
*	Side Effects: prints shell message
*/
int32_t term_write(int32_t fd, const void* buf, int32_t nbytes)
{	//printf("term write0\n");
	 int32_t i;
	 //uint8_t term_buf[_1KB]; /// ???????????????????????????????????????????????
	// buf = (uint8_t*) buf;
	// printf("input write buf ;%s \n",buf);
	//if (buf == NULL && *((uint8_t*)buf + strlen((int8_t*)buf)-1) != '\n'){
	//	
	//	return -1;
	//}
	if (buf == NULL || nbytes < 0)
		return -1;
	i = 0;
	//memset(term_buf, '\0', _1KB);
	//memcpy(term_buf, (uint8_t*)buf, (uint32_t)nbytes);
	//term_buf[nbytes] = '\0';
	//printf("term write1\n");
	do{
		if(*((uint8_t*)buf + i) == '\0')
			break;
		putc(*((uint8_t*)buf + i) );
		i++;
	}while(i<nbytes);

	return i;
}

/*   save_term
*    saves the details of the current terminal struct
*    Input: curr_term - current terminal to be saved
*    Output: N/A
*    Return: N/A
*    Effects: saves the current terminal info
*/
void save_term(term_t *curr_term){
	int i;
	curr_term->cursor_x = get_screen_x();
	curr_term->cursor_y = get_screen_y();
	memset(curr_term->vid_buf, '\0',VID_MEM_SIZE);
	for(i=0; i<VID_MEM_SIZE; i++)
	{
		curr_term->vid_buf[i] = *((uint8_t*)VMEM + i);
		if(!curr_term->vid_buf[i])
			break;
	}
	return;
}

/*   switch_term
*    switches the display term struct to the next terminal struct
*    Input: curr_term - current terminal to be saved
*    Output: N/A
*    Return: N/A
*    Effects: switches to the next terminal info
*/
void switch_term(term_t *next_term){
	int i;
	// !!!!! the 391OS> here is not a real shell for now, it's a fake printf that has to be integrated with paging !!!!!
	clear_screen();
	if(next_term->activated == 0)
	{
			//curr_exec_term = next_term->id;
			//execute((uint8_t*)"shell");
			next_term->activated = 1;
	}
	set_screen_xy(next_term->cursor_x, next_term->cursor_y);
	set_cursor_loc(next_term->cursor_x + next_term->cursor_y*NUM_COLS);
	//memset((uint8_t*)VMEM, '\0', VID_MEM_SIZE);
	for(i=0; i<VID_MEM_SIZE; i++)
	{
		*((uint8_t*)VMEM + i) = next_term->vid_buf[i];
		if(!next_term->vid_buf[i])
			break;
	}
	return;
}
