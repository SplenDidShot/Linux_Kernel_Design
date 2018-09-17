#ifndef _DEV_TERM_H
#define _DEV_TERM_H

#include "lib.h"
#include "types.h"
#include "dev_kbd.h"

#define KBD_BUF_SIZE		128
#define VID_MEM_SIZE		4096
#define NUM_COLS			80

typedef struct terminal {

	uint8_t kbd_line_cnt;
	uint8_t kbd_line_buf[KBD_BUF_SIZE];
	uint8_t kbd_out_buf[KBD_BUF_SIZE];
	uint8_t kbd_ready;
	uint8_t vid_buf[VID_MEM_SIZE];
	uint8_t activated;
	int  	cursor_x;
	int 	cursor_y;
	int32_t id;
    int32_t in_use_flag;
    struct pcb* term_pcb;
    int32_t num_program;

} term_t;

/* opens the terminal */
int32_t term_open(const uint8_t* filename);

/* closes the terminal */
int32_t term_close(int32_t fd);

/*  reads from the kbd message buffer */
int32_t term_read(int32_t fd, void* buf, int32_t nbytes);

/* writes from the terminal buffer */
int32_t term_write(int32_t fd, const void* buf, int32_t nbytes);

/* save the current terminal info*/
void save_term(term_t *curr_term);

/* switch to the target terminal info */
void switch_term(term_t *next_term);


#endif
