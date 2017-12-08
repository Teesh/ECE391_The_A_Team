#include "key.h"

unsigned int shift_flag = 0,ctrl_flag = 0,caps_flag = 0,alt_flag = 0,ins_flag = 0;
// lookup table for scan codes copied directly from online
// credit to Brandon F. at osdever.net
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
// modified lookup table for SHIFT
unsigned char kbdus_shift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* BACKSPACE */
  '\t',			/* TAB */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* ENTER KEY */
    0,			/* 29   - CONTROL */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '"', '~',   0,		/* LEFT SHIFT */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,				/* RIGHT SHIFT */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
// modified lookup table for CAPS
unsigned char kbdus_caps[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
// modified lookup table for SHIFT & CAPS
unsigned char kbdus_caps_shift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* backspace */
  '\t',			/* tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',	/* enter key */
    0,			/* 29   - control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',	/* 39 */
 '"', '~',   0,		/* left shift */
 '|', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', '<', '>', '?',   0,				/* right shift */
  '*',
    0,	/* alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
/*
	key_init - initialize keyboad
	inputs - none
	outputs - none
	side effects - keyboard resets, enables scanning, and sets scan codes to type 1
*/
void key_init() {
	// unsigned char temp;

	// outb(RESET,KEY_PORT);	
	// while((temp = inb(KEY_PORT)) != TEST_DONE);
		// printf("FAIL INIT KB\n");

	// outb(ENABLE,KEY_PORT);
	// while((temp = inb(KEY_PORT)) != KEY_ACK );
		// printf("FAIL INIT KB 1 \n");
	
	
	// outb(SCAN_ONE,KEY_PORT);
	// while((temp = inb(KEY_PORT)) != KEY_ACK );
		//printf("FAIL INIT KB 1 \n");
}

/*
	key_handler - handle keyboad interrupts
	inputs - none
	outputs - none
	side effects - prints out pressed character if printable
*/
void key_handler() {
	unsigned int code = 0;
	char key = 0;
	
	cli();
	outb(0x0A,VGA_INDEX);
	outb(0x0D,VGA_DATA);
	code = inb(KEY_PORT);//get code pressed
	// printf("%x",code);
	if(code == ESC) {
		if(term_live == 2 && term_pit == 2) {
			isPacman = 0;
			draw_background();
			halt(1);
		}
	}
	if(code >= ONE && code <= ZERO && isModex == 1 && isPacman == 0) {
		switch(code) {
			case ONE: paint_color = 0x03;
				break;
			case TWO: paint_color = 0x05;
				break;
			case THREE: paint_color = 0x02;
				break;
			case FOUR: paint_color = 0x06;
				break;
			case FIVE: paint_color = 0x04;
				break;
			case SIX: paint_color = 0x09;
				break;
			case SEVEN: paint_color = 0x08;
				break;
			case EIGHT: paint_color = 0x01;
				break;
			case NINE: paint_color = 0x07;
				break;
			case ZERO: paint_color = 0x0F;
				break;
		};
	}
	if(code >= SIZE_ONE && code <= SIZE_SIX && isModex == 1 && isPacman == 0) {
		switch(code) {
			case SIZE_ONE: paint_size = 1;
				break;
			case SIZE_TWO: paint_size = 2;
				break;
			case SIZE_THREE: paint_size = 3;
				break;
			case SIZE_FOUR: paint_size = 4;
				break;
			case SIZE_FIVE: paint_size = 5;
				break;
			case SIZE_SIX: paint_size = 6;
				break;
		};
	}
	if(code == LEFT_CTRL_DOWN) {
		ctrl_flag = 1;
	} else if(code == LEFT_CTRL_UP) {
		ctrl_flag = 0;
	}
	if(code == LEFT_SHIFT_DOWN || code == RIGHT_SHIFT_DOWN) {
		shift_flag = 1;
	} else if(code == LEFT_SHIFT_UP || code == RIGHT_SHIFT_UP) {
		shift_flag = 0;
	}
	if(code == LEFT_ALT_DOWN) {
		alt_flag = 1;
	} else if(code == LEFT_ALT_UP) {
		alt_flag = 0;
	}
	if(code == CAPS_LOCK) {
		if(caps_flag) {
			caps_flag = 0;
		} else {
			caps_flag = 1;
		}
	}
	if(code == INSERT) {
		background();
		start_gui();
	}
	if(code > 0x80) {
		// do nothing
	} else if(code == TAB) {
		tab_comp();
	} else if(code == UP) {
		hist_access(1);
	} else if(code == DOWN) {
		hist_access(0);
	} else if(code == LEFT) {
		buf_move(1);
	} else if(code == RIGHT) {
		buf_move(0);
	} else if(ctrl_flag == 1 && code == L_KEY) {
		if(isModex == 0 && isPacman == 0) {
			clear();
			cursor(0,0);
			term[term_live].screen_x=0;
			term[term_live].screen_y=0;
		} else if(isModex == 1 && isPacman == 0) {
			clear_paint();
		}
	} else if(ctrl_flag == 1 && code == C_KEY) {
		paint_background();
	} else if(ctrl_flag == 1) {
		// do nothing
	} else if(alt_flag == 1 && code == F1) {
			if(isPacman == 0) {
				int temp = term_live;
				term_live = 0;
				context_switch(temp,term_live);
				if(	isModex == 0){
					set_modex();
					draw_background();
				}
				else if (isPacman ==1 ){
					draw_background();
					isPacman = 0;
				}
				send_eoi(KEY_IRQ);
				sti();
				return;
			}
	} else if(alt_flag == 1 && code == F2) {
		if(isPacman == 0) {
			if(term_live == 2 && term[1].pcb_live == NULL){
				send_eoi(KEY_IRQ);
				sti();
				return;
			}
			if(	isModex == 1){
				set_text_mode_3(0);
				if(	isPacman == 1)
					isPacman = 0;
			}
			int temp = term_live;
			term_live = 1;
			context_switch(temp,term_live);
			send_eoi(KEY_IRQ);
			sti();
			return;
		}
	} else if(alt_flag == 1 && code == F3) {
		// if(	isModex == 1){
			// mouse_write(0xF5);
			// mouse_read();  //Acknowledge
			// set_text_mode_3(0);

		// }
		// int temp = term_live;
		// term_live = 2;
		// context_switch(temp,term_live);
	} else if(alt_flag == 1) {
		// do nothing
	
	} else if(shift_flag == 0 && caps_flag == 0) {
		key = kbdus[code];
	} else if(shift_flag == 0 && caps_flag == 1) {
		key = kbdus_caps[code];
	} else if(caps_flag == 0 && shift_flag == 1) {
		key = kbdus_shift[code];
	} else if(caps_flag == 1 && shift_flag == 1) {
		key = kbdus_caps_shift[code];
	}
	
	
	if(term_live == 2) {
		if(key!=NULL)//don't print when we release a key
		{
			if(key == 'a')
				last_key = KEY_LEFT;
			if(key == 'd')
				last_key = KEY_RIGHT;
			if(key == 's')
				last_key = KEY_DOWN;
			if(key == 'w')
				last_key = KEY_UP;
			if(key == 'l')
				isPlaying = 0;
			
		}
	}
	if(term_live == 1 && key != NULL) {
		key_to_ter(key);
	}
	
	send_eoi(KEY_IRQ);
	sti();
	return;
}

