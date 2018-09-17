#include "dev_kbd.h"

static uint8_t shift_flag;
static uint8_t caps_flag;
//static uint16_t curr_loc;
static uint8_t ctrl_flag;
static uint8_t alt_flag;

//terminal flag: 0 for 1st, 1 for 2nd, 2 for 3rd
 uint8_t term_flag;
// static term_t term_arr[term_flag];
term_t term_arr[3];

static char lower_char_table[KBD_TABLE_SIZE] = {
	 0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	0,
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	'\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',  0, '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   1,   1,   1,   1,   1,   1,
    1,	/* < ... F10 */
    1,	/* 69 - Num lock*/
    1,	/* Scroll Lock */
    1,	/* Home key */
    1,	/* Up Arrow */
    1,	/* Page Up */
  '-',
    1,	/* Left Arrow */
    1,
    1,	/* Right Arrow */
  '+',
    1,	/* 79 - End key*/
    1,	/* Down Arrow */
    1,	/* Page Down */
    1,	/* Insert Key */
    1,	/* Delete Key */
    1,   1,   1,
    1,	/* F11 Key */
    1,	/* F12 Key */
    1,	/* All other keys are undefined */
};

static char upper_char_table[KBD_TABLE_SIZE] = {
	 0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',	0,
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',	'\'', '`',   0,
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/',  0, '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   1,   1,   1,   1,   1,   1,
    1,	/* < ... F10 */
    1,	/* 69 - Num lock*/
    1,	/* Scroll Lock */
    1,	/* Home key */
    1,	/* Up Arrow */
    1,	/* Page Up */
  '-',
    1,	/* Left Arrow */
    1,
    1,	/* Right Arrow */
  '+',
    1,	/* 79 - End key*/
    1,	/* Down Arrow */
    1,	/* Page Down */
    1,	/* Insert Key */
    1,	/* Delete Key */
    1,   1,   1,
    1,	/* F11 Key */
    1,	/* F12 Key */
    1,	/* All other keys are undefined */
};

static char lowshift_char_table[KBD_TABLE_SIZE] = {
	 0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',	0,
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',	'\"', '~',   0,
 '|', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?',  0, '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   1,   1,   1,   1,   1,   1,
    1,	/* < ... F10 */
    1,	/* 69 - Num lock*/
    1,	/* Scroll Lock */
    1,	/* Home key */
    1,	/* Up Arrow */
    1,	/* Page Up */
  '-',
    1,	/* Left Arrow */
    1,
    1,	/* Right Arrow */
  '+',
    1,	/* 79 - End key*/
    1,	/* Down Arrow */
    1,	/* Page Down */
    1,	/* Insert Key */
    1,	/* Delete Key */
    1,   1,   1,
    1,	/* F11 Key */
    1,	/* F12 Key */
    1,	/* All other keys are undefined */
};

static char upshift_char_table[KBD_TABLE_SIZE] = {
	 0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	0,
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',  ':', '\"', '~',   0,
 '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',  0, '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   1,   1,   1,   1,   1,   1,
    1,	/* < ... F10 */
    1,	/* 69 - Num lock*/
    1,	/* Scroll Lock */
    1,	/* Home key */
    1,	/* Up Arrow */
    1,	/* Page Up */
  '-',
    1,	/* Left Arrow */
    1,
    1,	/* Right Arrow */
  '+',
    1,	/* 79 - End key*/
    1,	/* Down Arrow */
    1,	/* Page Down */
    1,	/* Insert Key */
    1,	/* Delete Key */
    1,   1,   1,
    1,	/* F11 Key */
    1,	/* F12 Key */
    1,	/* All other keys are undefined */
};

/*
* kbd_init
* DESC: calls the helper functions that adds kbd entry to the IDT and
*       enables the kbd IRQ line
* INPUT: kbd_irq_num - IRQ line of the kbd
* OUTPUT: N/A
* RETURN: N/A
* SIDE_EFFECTS: keyboard inputs could be printed on the terminal
*/
void kbd_init(uint32_t kbd_irq_num) {

	/* set runtime idt entry(0x21) */
	insert_idt(kbd_wrapper, kbd_irq_num);
	/* enables IRQ1 on Master PIC (0x21) */
	enable_irq(kbd_irq_num);

    //init flags
    int i;
    for(i=0; i<3; i++)
    {
        term_arr[i].kbd_line_cnt = 0;
        term_arr[i].kbd_ready = 0;
        term_arr[i].cursor_x = 0;
        term_arr[i].cursor_y = 0;
        term_arr[i].activated = 0;
        term_arr[i].id = i;
        term_arr[i].term_pcb=NULL;
        term_arr[i].num_program = 0;
        //printf("%s", term_arr[i].kbd_out_buf);
    }
    execute((uint8_t*)"shell");
    term_arr[0].activated = 1;
	shift_flag = 0;
	caps_flag = 0;
    alt_flag = 0;
    ctrl_flag = 0;

    // for(i=1; i<3; i++)
    // {
    //     term_flag = i;
    //     term_arr[i].kbd_ready = 1;
    //     term_arr[i].term_pcb = get_pcb();
    //     term_arr[i].kbd_ready = 0;
    // }
    // term_flag = 0;
}


/*
 * kbd_intr_handler
 * DESCRIPTION: keyboard interrupt handler-- prints the corresponding keyboard character on
 * the terminal according to input
 * I/O: N/A
 * RETURN: N/A
 * SIDE EFFECTS: prints characters and numbers on the console
 */

void kbd_intr_handler()   {
	//__asm__("pushal");

	uint8_t scancode;
	uint8_t c;
	cli();

	scancode = inb(KBD_DATA_PORT);
	/* decode the scancode to get ASCII char */
	c = scancode_decoder(scancode);

	if (ctrl_flag == 1) {
		//if (c == 'M' || c == 'm') {// use m to debug, ctrl-l is mapped to other scancode in this computer
		if (c == 'L' || c == 'l') {
		  clear_screen();
			set_cursor_loc(0);
		}
	}
	else {
		if (c == '\b')
			remove_op();
		else if (c != '\0' && c != 1)
			insert_op(c);
		else if (c == 1)
			puts("Key Code Not Supported\n391OS> ");
		set_cursor_loc((uint16_t)get_screen_loc());
	}

	send_eoi(1);
	sti();

//__asm__("popal; leave; iret");

}

/* remove_op
*  Input: N/A
*  Output: N/A
*  Return: N/A
*  Side Effects: deletes the last typed character and moves back cursor by one character
*/
void remove_op()
{
    term_arr[term_flag].kbd_ready = 0;
    if(term_arr[term_flag].kbd_line_cnt > 0)
    {
        term_arr[term_flag].kbd_line_cnt--;
        term_arr[term_flag].kbd_line_buf[term_arr[term_flag].kbd_line_cnt] = '\n';
    	erase_char();
	    set_cursor_loc((uint16_t)get_screen_loc());
    }
}

/*  insert_op
*   Input: c - the character to be printed
*   Output: puts the character on screen
*   Return: N/A
*   Side Effects: echoes the typed character on the terminal, 128 chars max per command
*/
void insert_op(uint8_t c)
{
    //checking the keyboard input buffer size on various inputs
	if(term_arr[term_flag].kbd_line_cnt >= KBD_BUF_SIZE - 1 && c != '\n') { //overflow handling
		term_arr[term_flag].kbd_line_buf[KBD_BUF_SIZE - 1] = '\n'; //missing before, this is necessary!!
        return;
	}
    else if(c == '\t' || c == '\0')
        return;
    else if(c == '\n')
    {
        term_arr[term_flag].kbd_line_buf[term_arr[term_flag].kbd_line_cnt] = c;
        term_arr[term_flag].kbd_line_cnt++;
        memcpy(term_arr[term_flag].kbd_out_buf, term_arr[term_flag].kbd_line_buf, KBD_BUF_SIZE);
        term_arr[term_flag].kbd_ready = 1;
        // uint8_t clr[KBD_BUF_SIZE];
        memset(term_arr[term_flag].kbd_line_buf, '\0', KBD_BUF_SIZE);
        term_arr[term_flag].kbd_line_cnt = 0;
        putc(c);
    }
    else
    {
        term_arr[term_flag].kbd_ready = 0;
        term_arr[term_flag].kbd_line_buf[term_arr[term_flag].kbd_line_cnt] = c;
        term_arr[term_flag].kbd_line_cnt++;
        putc(c);
    }

	set_cursor_loc((uint16_t)get_screen_loc());
}

/*  scancode_decoder
*   Input: sc - raw input scan code from the kbd
*   Output: converterted scancode into char
*   return: c- character from the scancode
*   Side Effects: N/A
*/
uint8_t scancode_decoder(uint8_t sc)   {

	uint8_t idx;
	uint8_t c;
    uint8_t old_flag;

	idx = sc;
	//kbd scan codes for nonprintable key: Lshift & Rshift left/right ctrl capslock and release
	if(idx == 0x1D)
		ctrl_flag = 1;
	else if (idx == 0x9D)
		ctrl_flag = 0;

    if(idx == 0x38)
        alt_flag = 1;
    else if (idx == 0xB8)
        alt_flag = 0;

	if(idx == 0x2A || idx == 0x36)
		shift_flag = 1;
	else if(idx == 0xAA || idx == 0xB6)
		shift_flag = 0;
	if(idx == 0x3A && caps_flag == 0)
		caps_flag = 1;
	else if (idx == 0x3A && caps_flag == 1)
		caps_flag = 0;
    //terminal switching logic alt+F1/F2/F3
    if(term_flag!=0 && alt_flag==1 && idx==0x3B)
    {
        save_term(&term_arr[term_flag]);
        old_flag = term_flag;
        term_flag = 0;
        switch_term(&term_arr[term_flag]);
        map_term(term_flag, old_flag, (int32_t*)term_arr[old_flag].vid_buf);
    }
    else if(term_flag!=1 && alt_flag==1 && idx==0x3C)
    {
        save_term(&term_arr[term_flag]);
        old_flag = term_flag;
        term_flag = 1;
        switch_term(&term_arr[term_flag]);
        map_term(term_flag, old_flag, (int32_t*)term_arr[old_flag].vid_buf);
    }
    else if(term_flag!=2 && alt_flag==1 && idx==0x3D)
    {
        save_term(&term_arr[term_flag]);
        old_flag = term_flag;
        term_flag = 2;
        switch_term(&term_arr[term_flag]);
        map_term(term_flag, old_flag, (int32_t*)term_arr[old_flag].vid_buf);
    }

	//set the output characters according to the shift and caps flags
	if (idx < KBD_TABLE_SIZE) {//has to add this to access the table
		if(shift_flag == 0 && caps_flag == 0)
			c = lower_char_table[idx];
		else if(shift_flag == 1 && caps_flag == 0)
			c = upshift_char_table[idx];
		else if(shift_flag == 0 && caps_flag == 1)
			c = upper_char_table[idx];
		else
			c = lowshift_char_table[idx];
	}
	else
		c = '\0';

	return c;

}
/*  is_kbd_ready
*   I/O: N/A
*   Returns: the status of the kbd buffer, ready or not
*   Side Effects: as long as enter key is pressed, ready would be set to high
*/
uint8_t is_kbd_ready(){
    return term_arr[term_flag].kbd_ready;
}
void unset_kbd_ready() {
	term_arr[term_flag].kbd_ready = 0;
}

/*  get_kbd_buf
*   I/O: N/A
*   Returns: the kbd output buffer
*   Side Effects: returns the kbd output buffer for terminal read to use
*/
uint8_t* get_kbd_buf(){
    return term_arr[term_flag].kbd_out_buf;
}
/*  get_term_pcb
*   returns the pcb of the current process on the target terminal
*   Input: next_term - next terminal number
*   Output: N/A
*   Return: pcb_t* of the next terminal
*   Side Effects: N/A
*/
struct pcb* get_term_pcb(uint8_t next_term){
    return term_arr[next_term].term_pcb;
}
