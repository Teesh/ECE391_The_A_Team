#include "modex.h"

 

/* Mode X and general VGA parameters */
int32_t mouse_x = 160, mouse_y = 100;
int32_t last_key = KEY_UP;
int32_t isPlaying = 0;
int32_t bg_flag = 0; 

/* local functions--see function headers for details */

static unsigned char* mem_image;    /* pointer to start of video memory */


unsigned char * build;
uint8_t paint_color = 0x01;
uint8_t paint_size = 3;

static int img3_off;		    /* offset of upper left pixel   */
static unsigned char* img3;	    /* pointer to upper left pixel  */
static int show_x, show_y;          /* logical view coordinates     */

static image_t head_img;
static image_t status_bar_img;
static image_t timer_img;
static image_t folder_icon_img;
static image_t pacman_icon;
static image_t paint_icon;
static image_t term_icon_img;
static image_t mouse_icon_img;
static image_t window_img1;
static image_t window_img2;
static image_t file_icon;
static image_t gif_window;
static image_t paint_win;

uint8_t isModex;
uint8_t isPacman;

unsigned char * build_ptr;

static unsigned char store_buf_pacman[8*8] = {
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};

static unsigned char store_buf_ghost[8*8] = {
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};



///???//
void clear_buffer();
image_t* head_img_ptr;
image_t* tail_img_ptr;

/*  
 * macro used to target a specific video plane or planes when writing
 * to video memory in mode X; bits 8-11 in the mask_hi_bits enable writes
 * to planes 0-3, respectively
 */
#define SET_WRITE_MASK(mask_hi_bits)                                    \
do {                                                                    \
    asm volatile ("                                                     \
	movw $0x03C4,%%dx    	/* set write mask                    */;\
	movb $0x02,%b0                                                 ;\
	outw %w0,(%%dx)                                                 \
    " : : "a" ((mask_hi_bits)) : "edx", "memory");                      \
} while (0)

/* macro used to write a byte to a port */
#define OUTB(port,val)                                                  \
do {                                                                    \
    asm volatile ("                                                     \
        outb %b1,(%w0)                                                  \
    " : /* no outputs */                                                \
      : "d" ((port)), "a" ((val))                                       \
      : "memory", "cc");                                                \
} while (0)

/* macro used to write two bytes to two consecutive ports */
#define OUTW(port,val)                                                  \
do {                                                                    \
    asm volatile ("                                                     \
        outw %w1,(%w0)                                                  \
    " : /* no outputs */                                                \
      : "d" ((port)), "a" ((val))                                       \
      : "memory", "cc");                                                \
} while (0)

/* 
 * macro used to write an array of two-byte values to two consecutive ports 
 */
#define REP_OUTSW(port,source,count)                                    \
do {                                                                    \
    asm volatile ("                                                     \
     1: movw 0(%1),%%ax                                                ;\
	outw %%ax,(%w2)                                                ;\
	addl $2,%1                                                     ;\
	decl %0                                                        ;\
	jne 1b                                                          \
    " : /* no outputs */                                                \
      : "c" ((count)), "S" ((source)), "d" ((port))                     \
      : "eax", "memory", "cc");                                         \
} while (0)

/* 
 * macro used to write an array of one-byte values to two consecutive ports 
 */
#define REP_OUTSB(port,source,count)                                    \
do {                                                                    \
    asm volatile ("                                                     \
     1: movb 0(%1),%%al                                                ;\
	outb %%al,(%w2)                                                ;\
	incl %1                                                        ;\
	decl %0                                                        ;\
	jne 1b                                                          \
    " : /* no outputs */                                                \
      : "c" ((count)), "S" ((source)), "d" ((port))                     \
      : "eax", "memory", "cc");                                         \
} while (0)
	

/* VGA register settings for mode X */
static unsigned short mode_X_seq[NUM_SEQUENCER_REGS] = {
    0x0100, 0x2101, 0x0F02, 0x0003, 0x0604
};
static unsigned short mode_X_CRTC[NUM_CRTC_REGS] = {
    0x5F00, 0x4F01, 0x5002, 0x8203, 0x5404, 0x8005, 0xBF06, 0x1F07,
    0x0008, 0x4109, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x9C10, 0x8E11, 0x8F12, 0x2813, 0x0014, 0x9615, 0xB916, 0xE317,
    0xFF18
};
static unsigned char mode_X_attr[NUM_ATTR_REGS * 2] = {
    0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 
    0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 
    0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B, 
    0x0C, 0x0C, 0x0D, 0x0D, 0x0E, 0x0E, 0x0F, 0x0F,
    0x10, 0x41, 0x11, 0x00, 0x12, 0x0F, 0x13, 0x00,
    0x14, 0x00, 0x15, 0x00
};
static unsigned short mode_X_graphics[NUM_GRAPHICS_REGS] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x4005, 0x0506, 0x0F07,
    0xFF08
};

/* VGA register settings for text mode 3 (color text) */
static unsigned short text_seq[NUM_SEQUENCER_REGS] = {
    0x0100, 0x2001, 0x0302, 0x0003, 0x0204
};
static unsigned short text_CRTC[NUM_CRTC_REGS] = {
    0x5F00, 0x4F01, 0x5002, 0x8203, 0x5504, 0x8105, 0xBF06, 0x1F07,
    0x0008, 0x4F09, 0x0D0A, 0x0E0B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x9C10, 0x8E11, 0x8F12, 0x2813, 0x1F14, 0x9615, 0xB916, 0xA317,
    0xFF18
};
static unsigned char text_attr[NUM_ATTR_REGS * 2] = {
    0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 
    0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 
    0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B, 
    0x0C, 0x0C, 0x0D, 0x0D, 0x0E, 0x0E, 0x0F, 0x0F,
    0x10, 0x0C, 0x11, 0x00, 0x12, 0x0F, 0x13, 0x08,
    0x14, 0x00, 0x15, 0x00
};
static unsigned short text_graphics[NUM_GRAPHICS_REGS] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x1005, 0x0E06, 0x0007,
    0xFF08
};

void clock(uint32_t sec) {
	if(isModex == 0 || isPacman == 1)
		return;
	int i;
	char inp2[6] = "00:00";
	unsigned char buf2[40 * 16];
	uint32_t ten_sec,one_sec,ten_min,one_min;
	static int counter = 0;
	
	for(i = 0 ; i <40 * 16; i ++){
		buf2[i] = 0x0;
	}
	
	one_sec = (sec%60)%10;
	ten_sec = (sec%60)/10;
	one_min = (sec/60)%10;
	ten_min = (sec/60)/10;
	
	inp2[4] = one_sec + '0';
	inp2[3] = ten_sec + '0';
	inp2[1] = one_min + '0';
	inp2[0] = ten_min + '0';
	
	create_block_text_timer(buf2, inp2);
	for (i = 0 ; i < 16 * 40 ; i++ ){
			if(buf2[i] != 0)
				buf2[i] = 0xF;
	}
	memcpy(timer_block,buf2,40*16);
	
	image_t* temp;
	temp = &status_bar_img;
	uint8_t mouse_store_buffer[8*8];
	uint8_t redraw_mouse = 0;
	int temp_mouse_x ;
	int temp_mouse_y ;
	
	
	if(gif_window.next != NULL) {
		for(i=0;i<158;i++) {
			switch(counter) {
				case 0: memcpy(gif_viewer+(i+10)*129+2,frames[0]+i*125,125);
					break;
				case 24: memcpy(gif_viewer+(i+10)*129+2,frames[1]+i*125,125);
					break;
				case 48: memcpy(gif_viewer+(i+10)*129+2,frames[2]+i*125,125);
					break;
				case 72: memcpy(gif_viewer+(i+10)*129+2,frames[3]+i*125,125);
					break;
				case 96: memcpy(gif_viewer+(i+10)*129+2,frames[4]+i*125,125);
					break;
			};
		}
	}
	counter++;
	counter %= 120;
	
	cli();
	while(temp != NULL){
		if(temp->next == NULL){
			for(i = 0 ;i < 64; i ++){
				mouse_store_buffer[i] = 0;
			}
			temp_mouse_x = temp->start_x;
			temp_mouse_y = temp->start_y;
			draw_save_full_block(temp->matrix_ptr, mouse_store_buffer, temp_mouse_x, temp_mouse_y, temp->width, temp->height, 1);
			redraw_mouse = 1;
			break;
		}
		draw_full_block(temp->start_x, temp->start_y, temp->matrix_ptr, temp->width, temp->height);
		temp = temp->next;
	}

	if(isModex == 1 && isPacman == 0) {
		show_screen();		
		if(redraw_mouse == 1)
			draw_save_full_block(mouse_store_buffer, NULL, temp_mouse_x, temp_mouse_y, temp->width, temp->height, 0);	
	}
	sti();
	return;
}
void init_modex(){
	build_ptr = build;
	
	head_img_ptr = &head_img;
	tail_img_ptr = &mouse_icon_img;
	
	head_img.id = 0;
	head_img.matrix_ptr = background_img;
	head_img.width = 320;
	head_img.height = 200;
	head_img.start_x = 0;
	head_img.start_y = 0;
	head_img.next = &status_bar_img;
	head_img.prev = NULL;
	
	status_bar_img.id = 0;
	status_bar_img.matrix_ptr = status_bar;
	status_bar_img.width = 320;
	status_bar_img.height = 18;
	status_bar_img.start_x = 0;
	status_bar_img.start_y = 182;
	status_bar_img.next = &timer_img;
	status_bar_img.prev = &head_img;

	timer_img.id = 0;
	timer_img.matrix_ptr = timer_block;
	timer_img.width = 40;
	timer_img.height = 16;
	timer_img.start_x = 260;
	timer_img.start_y = 183;
	timer_img.next = &term_icon_img;
	timer_img.prev = &status_bar_img;

	term_icon_img.id = 1;
	term_icon_img.matrix_ptr = terminal_img;
	term_icon_img.width = 32;
	term_icon_img.height = 20;
	term_icon_img.start_x = 20;
	term_icon_img.start_y = 20;
	term_icon_img.next = &folder_icon_img;
	term_icon_img.prev = &timer_img;
	
	folder_icon_img.id = 2;
	folder_icon_img.matrix_ptr = folder_img;
	folder_icon_img.width = 32;
	folder_icon_img.height = 20;
	folder_icon_img.start_x = 60;
	folder_icon_img.start_y = 20;
	folder_icon_img.next = &pacman_icon;
	folder_icon_img.prev = &term_icon_img;
	
	pacman_icon.id = 6;
	pacman_icon.matrix_ptr = pacman_img;
	pacman_icon.width = 32;
	pacman_icon.height = 20;
	pacman_icon.start_x = 100;
	pacman_icon.start_y = 20;
	pacman_icon.next = &paint_icon;
	pacman_icon.prev = &folder_icon_img;
	
	paint_icon.id = 9;
	paint_icon.matrix_ptr = paint_img;
	paint_icon.width = 32;
	paint_icon.height = 20;
	paint_icon.start_x = 140;
	paint_icon.start_y = 20;
	paint_icon.next = &mouse_icon_img;
	paint_icon.prev = &pacman_icon;
	
	mouse_icon_img.id = 3;
	mouse_icon_img.matrix_ptr = mouse_img;
	mouse_icon_img.width = 8;
	mouse_icon_img.height = 8;
	mouse_icon_img.start_x = 160;
	mouse_icon_img.start_y = 100;
	mouse_icon_img.next = NULL;	
	mouse_icon_img.prev = &paint_icon;	

	window_img1.id = 4;
	window_img1.matrix_ptr = window_img;
	window_img1.width = 160;
	window_img1.height = 100;
	window_img1.start_x = 20;
	window_img1.start_y = 40;
	window_img1.next = NULL;	
	window_img1.prev = NULL;
	
	window_img2.id = 5;
	window_img2.matrix_ptr = window_img;
	window_img2.width = 160;
	window_img2.height = 100;
	window_img2.start_x = 40;
	window_img2.start_y = 60;
	window_img2.next = NULL;	
	window_img2.prev = NULL;	
	
	file_icon.id = 7;
	file_icon.matrix_ptr = file_img;
	file_icon.width = 20;
	file_icon.height = 32;
	file_icon.start_x = 26;
	file_icon.start_y = 66;
	file_icon.next = NULL;	
	file_icon.prev = NULL;

	gif_window.id = 8;
	gif_window.matrix_ptr = gif_viewer;
	gif_window.width = 129;
	gif_window.height = 170;
	gif_window.start_x = 60;
	gif_window.start_y = 40;
	gif_window.next = NULL;	
	gif_window.prev = NULL;	
	
	paint_win.id = 10;
	paint_win.matrix_ptr = paint_window;
	paint_win.width = 320;
	paint_win.height = 182;
	paint_win.start_x = 0;
	paint_win.start_y = 0;
	paint_win.next = NULL;	
	paint_win.prev = NULL;	
}

void background(void) {
	if(isModex == 1 && isPacman == 0) {
		if(!bg_flag) {
			file_open("img.raw");
			file_read(-1,background_img,320*200);
			bg_flag = 1;
		} else {
			file_open("img1.raw");
			file_read(-1,background_img,320*200);
			bg_flag = 0;
		}
	}
}

void add_img() {
	if(window_img1.prev == NULL) {
		window_img1.next = &file_icon;
		window_img1.prev = tail_img_ptr->prev;
		file_icon.next = tail_img_ptr;
		file_icon.prev = &window_img1;
		tail_img_ptr->prev->next = &window_img1;
		tail_img_ptr->prev = &file_icon;
	} else if(window_img2.prev == NULL) {
		window_img2.next = tail_img_ptr;
		window_img2.prev = tail_img_ptr->prev;
		tail_img_ptr->prev->next = &window_img2;
		tail_img_ptr->prev = &window_img2;
	}
}

void start_gif() {
	gif_window.next = tail_img_ptr;
	gif_window.prev = tail_img_ptr->prev;
	tail_img_ptr->prev->next = &gif_window;
	tail_img_ptr->prev = &gif_window;
}

void start_paint() {
	paint_win.next = tail_img_ptr;
	paint_win.prev = tail_img_ptr->prev;
	tail_img_ptr->prev->next = &paint_win;
	tail_img_ptr->prev = &paint_win;
}

void draw_paint(uint32_t x,uint32_t y) {
	int i,j;

	for(i=0;i<paint_size;i++) {
		for(j=0;j<paint_size;j++) {
			if((x+j-paint_size)>0 && (y+i-paint_size)>0) *((uint8_t*)((uint32_t)paint_window+(x+j-paint_size)+((y+i-paint_size)*320))) = paint_color;
		}
	}
}

void clear_paint() {
	int i;
	for(i=0;i<paint_win.height-12;i++) {
		memset((uint8_t*)((uint32_t)paint_window+(i*320)+(320*10)+2),0x0f,316);
	}
}

void paint_background() {
	int i;
	memset(background_img,0x0f,64000);
	for(i=0;i<paint_win.height-12;i++) {
		memcpy((uint8_t*)((uint32_t)background_img+(i*320)+(320*6)+2),(uint8_t*)((uint32_t)paint_window+(i*320)+(320*10)+2),316);
	}
}

void move_window(int id, int32_t del_x, int32_t del_y){
	if(id == 0 || id == 3) return;
	if(del_x != 0 || del_y != 0) left_drag_flag = 1;
	switch(id) {
		case 1: if(!(mouse_x <= 0 || mouse_x >= 319)) 
					term_icon_img.start_x += del_x;
			if(!(mouse_y <= 0 || mouse_y >= 199))
				term_icon_img.start_y -= del_y;
			break;
		case 2: if(!(mouse_x <= 0 || mouse_x >= 319))
					folder_icon_img.start_x += del_x;
			if(!(mouse_y <= 0 || mouse_y >= 199))
				folder_icon_img.start_y -= del_y;
			break;
		case 4: if(!(mouse_x <= 0 || mouse_x >= 319))
					window_img1.start_x += del_x;
					file_icon.start_x += del_x;
			if(!(mouse_y <= 0 || mouse_y >= 199))
				window_img1.start_y -= del_y;
				file_icon.start_y -= del_y;
			break;
		case 5: if(!(mouse_x <= 0 || mouse_x >= 319))
					window_img2.start_x += del_x;
			if(!(mouse_y <= 0 || mouse_y >= 199))
				window_img2.start_y -= del_y;
			break;
		case 6: if(!(mouse_x <= 0 || mouse_x >= 319))
					pacman_icon.start_x += del_x;
			if(!(mouse_y <= 0 || mouse_y >= 199))
				pacman_icon.start_y -= del_y;
			break;
		case 7: if(!(mouse_x <= 0 || mouse_x >= 319) && 
				!(file_icon.start_x+del_x <= window_img1.start_x+2 || file_icon.start_x+file_icon.width+del_x >= window_img1.start_x+window_img1.width-2))
					file_icon.start_x += del_x;
				if(!(mouse_y <= 0 || mouse_y >= 319) && 
				!(file_icon.start_y-del_y <= window_img1.start_y+10 || file_icon.start_y+file_icon.height-del_y >= window_img1.start_y+window_img1.height-2))
					file_icon.start_y -= del_y;
			break;
		case 8: if(!(mouse_x <= 0 || mouse_x >= 319))
					gif_window.start_x += del_x;
			if(!(mouse_y <= 0 || mouse_y >= 199))
				gif_window.start_y -= del_y;
			break;
		case 9: if(!(mouse_x <= 0 || mouse_x >= 319))
					paint_icon.start_x += del_x;
			if(!(mouse_y <= 0 || mouse_y >= 199))
				paint_icon.start_y -= del_y;
			break;
		case 10: if(!(mouse_x <= 0 || mouse_x >= 319))
				paint_win.start_x += del_x;
			if(!(mouse_y <= 0 || mouse_y >= 199))
				paint_win.start_y -= del_y;
			break;
		default:
			break;
	}
	return;
}

void bring_to_front(int id){
	if(id == 0 || id == 3)
		return;
	if(tail_img_ptr->prev->id == id ){
		return;
	}
	
	image_t* temp;
	temp = &head_img;
	while(temp!= NULL){
		if(temp->id == id) break;
		temp = temp->next;
	}
	
	if(temp == NULL || temp->next==NULL) return;
	temp->prev->next = temp->next;
	temp->next->prev = temp->prev;
	
	temp->next = tail_img_ptr;
	temp->prev = tail_img_ptr->prev;
	temp->prev->next = temp;
	temp->next->prev = temp;
	
	if(id == 4) {
		temp = &head_img;
		while(temp!= NULL){
			if(temp->id == 7) {
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				
				temp->next = tail_img_ptr;
				temp->prev = tail_img_ptr->prev;
				temp->prev->next = temp;
				temp->next->prev = temp;
			}
			temp = temp->next;
		}
	}
}

extern void close_window(int id) {
	image_t* temp;
	temp = &head_img;
	while(temp!= NULL){
		if(temp->id == id) break;
		temp = temp->next;
	}
	if(temp == NULL || temp->next==NULL) return;
	
	temp->prev->next = temp->next;
	temp->next->prev = temp->prev;
	temp->next = NULL;
	temp->prev = NULL;
	
	if(id == 4) {
		temp = &head_img;
		while(temp!= NULL){
			if(temp->id == 7) break;
			temp = temp->next;
		}
		if(temp == NULL || temp->next==NULL) return;
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		temp->next = NULL;
		temp->prev = NULL;
	}
	return;
}

void start_splash(){
	unsigned char buf[TEXT_BLK_X_DIM*2 * TEXT_BLK_Y_DIM*2];
	int i;
	
	for(i = 0 ; i <TEXT_BLK_X_DIM*2 * TEXT_BLK_Y_DIM*2; i ++){
		buf[i] = 0x0;
	}
	
	char inp[11] = "THE K TEAM";
	create_block_text_double(buf, inp);
	draw_full_block(80,68,buf, TEXT_BLK_X_DIM*2, TEXT_BLK_Y_DIM*2);

	show_screen();
	int count = 0;
	char fake_buf[2];

	while( count < 31){
		ateam(count);
		for (i = 0 ; i < TEXT_BLK_X_DIM*2 * TEXT_BLK_Y_DIM*2 ; i++ ){
			if(buf[i] != 0)
				buf[i]++;
		}
		rtc_read(-1,fake_buf, 1 );
		count++;
		draw_full_block(80,68,buf, TEXT_BLK_X_DIM*2, TEXT_BLK_Y_DIM*2);
		
		show_screen();
	}
	
	clear_buffer();
}

void clear_buffer(){
	int i; 
	for (i = 0 ; i <BUILD_BUF_SIZE; i ++ ){
		build[i] = 0;
	}
	
}


/*
 * show_screen
 *   DESCRIPTION: Show the logical view window on the video display.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: copies from the build buffer to video memory;
 *                 shifts the VGA display source to point to the new image
 */   
void
show_screen ()
{
	unsigned char* addr;  /* source address for copy             */
    int p_off;            /* plane offset of first display plane */
    int i;		  /* loop index over video planes        */

    /* 
     * Calculate offset of build buffer plane to be mapped into plane 0 
     * of display.
     */
    p_off = (3 - (show_x & 3));


    /* Calculate the source address. */
    addr = img3 + (show_x >> 2) + show_y * SCROLL_X_WIDTH;

    /* Draw to each plane in the video memory. */
    for (i = 0; i < 4; i++) {
		SET_WRITE_MASK (1 << (i + 8));
		copy_image (addr + ((p_off - i + 4) & 3) * SCROLL_SIZE + (p_off < i));
		
    }
}

uint32_t parse_pacman(void * pacman_in, int32_t flag) {
	pacman_t * pacman;
	if(isPacman == 0)
		return 0;
	
	
	// uint32_t * test;
	// int i_test;
	// uint32_t buf_addr = (uint32_t)&build;
	// uint32_t buf_up = buf_addr + BUILD_BUF_SIZE;
	
	
	// test = (uint32_t*) term[0].saved_ebp;
	// i_test = *test;
	// if(i_test < FOURKILO){
		// i_test = 0;
	// }
	
	
	if(pacman_in != NULL) pacman = (pacman_t*)pacman_in;
	switch (flag){
		case 0:
			draw_save_full_block(wall_0_block, NULL, pacman->maze_x*8, pacman->maze_y*8, 8,8,0);	// make this a system call
			break;
		case 1:
			draw_save_full_block( wall_1_block, NULL,pacman->maze_x*8, pacman->maze_y*8,  8,8,0);
			break;
		case 2:
			draw_save_full_block( wall_2_block, NULL,pacman->maze_x*8, pacman->maze_y*8,  8,8,0);
			break;
		case 3:
			draw_save_full_block( wall_3_block, NULL, pacman->maze_x*8, pacman->maze_y*8, 8,8,0);
			break;
		case 4:
			draw_save_full_block(wall_4_block,  NULL,pacman->maze_x*8, pacman->maze_y*8, 8,8,0);
			break;
		case 5:
			draw_save_full_block( wall_5_block, NULL, pacman->maze_x*8, pacman->maze_y*8, 8,8,0);
			break;
		case 6:
			draw_save_full_block( wall_6_block, NULL,pacman->maze_x*8, pacman->maze_y*8, 8,8,0);
			break;
		case 7:
			draw_save_full_block(wall_7_block,  NULL,pacman->maze_x*8, pacman->maze_y*8, 8,8,0);
			break;
		case 8:
		///////////
			// test =  term[0].saved_ebp;
			// if(test <= buf_up && test>=buf_addr  )
				// i_test = 0;
		// 
			clear_buffer();
			break;
		case 9:
			show_screen();
			break;
		case 10:
			if(isPacman==0 || isModex == 0)
				break;
			show_small_block(store_buf_pacman, pacman->pos_x, pacman->pos_y);
			draw_save_full_block(pacman_block, store_buf_pacman, pacman->pos_x_in, pacman->pos_y_in, 8,8, 1);
			draw_save_full_block(store_buf_pacman, NULL, pacman->pos_x_in, pacman->pos_y_in, 8,8, 0);
			show_small_block(pacman_block, pacman->pos_x_in, pacman->pos_y_in);
			beep(100,1,1);
			beep(50,1,1);
			break;
		case 11:
			draw_save_full_block(pacman_block_up, store_buf_pacman, pacman->pos_x_in, pacman->pos_y_in, 8,8, 1);
			break;
		case 12:
			draw_save_full_block(pacman_block_down, store_buf_pacman, pacman->pos_x_in, pacman->pos_y_in, 8,8, 1);
			break;
		case 13:
			draw_save_full_block(pacman_block_right, store_buf_pacman, pacman->pos_x_in, pacman->pos_y_in, 8,8, 1);
			break;
		case 14:
			draw_save_full_block(pacman_block_left, store_buf_pacman, pacman->pos_x_in, pacman->pos_y_in, 8,8, 1);
			break;
		case 15:
			draw_save_full_block(wall_0_block, NULL, pacman->pos_x_in, pacman->pos_y_in, 8,8, 0);
			break;
		case 16:
			draw_save_full_block(store_buf_pacman, NULL, pacman->pos_x_in, pacman->pos_y_in, 8,8, 0);
			break;
		case 17:
			show_small_block(pacman_block_up, pacman->pos_x_in, pacman->pos_y_in);
			break;
		case 18:
			show_small_block(pacman_block_down, pacman->pos_x_in, pacman->pos_y_in);
			break;
		case 19:
			show_small_block(pacman_block_right, pacman->pos_x_in, pacman->pos_y_in);
			break;
		case 20:
			show_small_block(pacman_block_left, pacman->pos_x_in, pacman->pos_y_in);
			break;
		case 21:
			show_small_block(store_buf_ghost, pacman->pos_x, pacman->pos_y);
			break;
		case 22:
			draw_save_full_block(ghost_block, store_buf_ghost, pacman->pos_x_in, pacman->pos_y_in, 8,8, 1);
			break;
		case 23:
			draw_save_full_block(store_buf_ghost, NULL, pacman->pos_x_in, pacman->pos_y_in, 8,8, 0);
			break;
		case 24:
			show_small_block(ghost_block, pacman->pos_x_in, pacman->pos_y_in);
			break;
		case 25:
			show_small_block(store_buf_pacman, pacman->pos_x, pacman->pos_y);
			break;
		case 26:
			draw_lose_pacman();
			beep(300,50,50);
			beep(200,50,50);
			beep(100,50,50);
			break;
		case 27:
			draw_win_pacman();
			break;

		default:
			break;
	}
	
	
	// test = (uint32_t*) term[0].saved_ebp;
	// i_test = *test;
	// if(i_test < FOURKILO){
		// i_test = 0;
	// }
	
	
	
	return last_key;
}
void draw_lose_pacman(){
	draw_full_block(0,0, pacman_fail, 320,200);
	show_screen();
}
void draw_win_pacman(){
	
	
}




void start_gui(void) {
	image_t* temp;
	temp = &head_img;
	while(temp->next != NULL){
		draw_full_block(temp->start_x, temp->start_y, temp->matrix_ptr, temp->width, temp->height);
		temp = temp->next;
	}

	show_screen();
}



int set_modex(){
	int i ;
	    /* Map video memory and obtain permission for VGA port access. */
   
 /* 
     * The code below was produced by recording a call to set mode 0013h
     * with display memory clearing and a windowed frame buffer, then
     * modifying the code to set mode X instead.  The code was then
     * generalized into functions...
     *
     * modifications from mode 13h to mode X include...
     *   Sequencer Memory Mode Register: 0x0E to 0x06 (0x3C4/0x04)
     *   Underline Location Register   : 0x40 to 0x00 (0x3D4/0x14)
     *   CRTC Mode Control Register    : 0xA3 to 0xE3 (0x3D4/0x17)
     */
	isModex = 1;
	mem_image = (void *) 0xA0000;
    show_x = show_y = 0;
    img3_off = 0;
	img3 = build + img3_off ;
	for (i = 0; i < BUILD_BUF_SIZE; i++) {
        build[i] = 0x00;
    }
	
    VGA_blank (1);                               /* blank the screen      */
    set_seq_regs_and_reset (mode_X_seq, 0x63);   /* sequencer registers   */
    set_CRTC_registers (mode_X_CRTC);            /* CRT control registers */
    set_attr_registers (mode_X_attr);            /* attribute registers   */
    set_graphics_registers (mode_X_graphics);    /* graphics registers    */
    fill_palette ();				 /* palette colors        */
    clear_screens ();				 /* zero video memory     */
    VGA_blank (0);			         /* unblank the screen    */

    /* Return success. */
    return 0;
}


/*
 * clear_mode_X
 *   DESCRIPTION: Puts the VGA into text mode 3 (color text).
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: restores font data to video memory; clears screens;
 *                 unmaps video memory; checks memory fence integrity
 */   
void clear_mode_X ()
{
    // int i;   /* loop index for checking memory fence */
    
    /* Put VGA into text mode, restore font data, and clear screens. */
    set_text_mode_3 (1);

	
    // /* Check validity of build buffer memory fence.  Report breakage. */
    // for (i = 0; i < MEM_FENCE_WIDTH; i++) {
	// if (build[i] != MEM_FENCE_MAGIC) {
	    // puts ("lower build fence was broken");
	    // break;
	// }
    // }
    // for (i = 0; i < MEM_FENCE_WIDTH; i++) {
        // if (build[BUILD_BUF_SIZE + MEM_FENCE_WIDTH + i] != MEM_FENCE_MAGIC) {
	    // puts ("upper build fence was broken");
	    // break;
	// }
    // }
}

/*
 * VGA_blank
 *   DESCRIPTION: Blank or unblank the VGA display.
 *   INPUTS: blank_bit -- set to 1 to blank, 0 to unblank
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */   
void VGA_blank (int blank_bit)
{
    /* 
     * Move blanking bit into position for VGA sequencer register 
     * (index 1). 
     */
    blank_bit = ((blank_bit & 1) << 5);
    asm volatile (
	"movb $0x01,%%al         /* Set sequencer index to 1. */       ;"
	"movw $0x03C4,%%dx                                             ;"
	"outb %%al,(%%dx)                                              ;"
	"incw %%dx                                                     ;"
	"inb (%%dx),%%al         /* Read old value.           */       ;"
	"andb $0xDF,%%al         /* Calculate new value.      */       ;"
	"orl %0,%%eax                                                  ;"
	"outb %%al,(%%dx)        /* Write new value.          */       ;"
	"movw $0x03DA,%%dx       /* Enable display (0x20->P[0x3C0]) */ ;"
	"inb (%%dx),%%al         /* Set attr reg state to index. */    ;"
	"movw $0x03C0,%%dx       /* Write index 0x20 to enable. */     ;"
	"movb $0x20,%%al                                               ;"
	"outb %%al,(%%dx)                                               "
      : : "g" (blank_bit) : "eax", "edx", "memory");
}

/*
 * set_seq_regs_and_reset
 *   DESCRIPTION: Set VGA sequencer registers and miscellaneous output
 *                register; array of registers should force a reset of
 *                the VGA sequencer, which is restored to normal operation
 *                after a brief delay.
 *   INPUTS: table -- table of sequencer register values to use
 *           val -- value to which miscellaneous output register should be set
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */   
void set_seq_regs_and_reset (unsigned short table[NUM_SEQUENCER_REGS],
			unsigned char val)
{
    /* 
     * Dump table of values to sequencer registers.  Includes forced reset
     * as well as video blanking.
     */
    REP_OUTSW (0x03C4, table, NUM_SEQUENCER_REGS);

    /* Delay a bit... */
    {volatile int ii; for (ii = 0; ii < 10000; ii++);}

    /* Set VGA miscellaneous output register. */
    OUTB (0x03C2, val);

    /* Turn sequencer on (array values above should always force reset). */
    OUTW (0x03C4,0x0300);
}


/*
 * set_CRTC_registers
 *   DESCRIPTION: Set VGA cathode ray tube controller (CRTC) registers.
 *   INPUTS: table -- table of CRTC register values to use
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */   
void set_CRTC_registers (unsigned short table[NUM_CRTC_REGS])
{
    /* clear protection bit to enable write access to first few registers */
    OUTW (0x03D4, 0x0011); 
    REP_OUTSW (0x03D4, table, NUM_CRTC_REGS);
}


/*
 * set_attr_registers
 *   DESCRIPTION: Set VGA attribute registers.  Attribute registers use
 *                a single port and are thus written as a sequence of bytes
 *                rather than a sequence of words.
 *   INPUTS: table -- table of attribute register values to use
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */   
void set_attr_registers (unsigned char table[NUM_ATTR_REGS * 2])
{
    /* Reset attribute register to write index next rather than data. */
    asm volatile (
	"inb (%%dx),%%al"
      : : "d" (0x03DA) : "eax", "memory");
    REP_OUTSB (0x03C0, table, NUM_ATTR_REGS * 2);
}

/*
 * set_graphics_registers
 *   DESCRIPTION: Set VGA graphics registers.
 *   INPUTS: table -- table of graphics register values to use
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */   
void set_graphics_registers (unsigned short table[NUM_GRAPHICS_REGS])
{
    REP_OUTSW (0x03CE, table, NUM_GRAPHICS_REGS);
}

/*
 * fill_palette
 *   DESCRIPTION: Fill VGA palette with necessary colors for the maze game.
 *                Only the first 64 (of 256) colors are written.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes the first 64 palette colors
 */   
void fill_palette ()
{
	
	    /* 6-bit RGB (red, green, blue) values for first 64 colors */
    static unsigned char palette_RGB[64][3] = {
		{0x00, 0x00, 0x00}, {0x34, 0x34, 0x34},   /* palette 0x00 - 0x0F    */
		{0x07, 0x30, 0x00}, {0x00, 0x00, 0x00},   /* basic VGA colors       */
		{0x3F, 0x3F, 0x00}, {0x3F, 0x00, 0x00},	
		{0x00, 0x00, 0x3F}, {0x2A, 0x2A, 0x2A}, //	Used 0-6 and 0xF
		{0x1F, 0x00, 0x1F}, {0x3F, 0x1F, 0x00},
		
		
		
		{0x00, 0x3F, 0x00}, {0x3F, 0x00, 0x00},
		{0x00, 0x00, 0x3F}, {0x3F, 0x15, 0x3F},
		
		
		
		{0x0D, 0x1B, 0x3F}, {0x3F, 0x3F, 0x3F},
		
		
		{0x00, 0x00, 0x00}, {0x05, 0x05, 0x05},   /* palette 0x10 - 0x1F    */
		{0x08, 0x08, 0x08}, {0x0B, 0x0B, 0x0B},   /* VGA grey scale         */
		{0x0E, 0x0E, 0x0E}, {0x11, 0x11, 0x11},
		{0x14, 0x14, 0x14}, {0x18, 0x18, 0x18},
		{0x1C, 0x1C, 0x1C}, {0x20, 0x20, 0x20},
		{0x24, 0x24, 0x24}, {0x28, 0x28, 0x28},
		{0x2D, 0x2D, 0x2D}, {0x32, 0x32, 0x32},
		{0x38, 0x38, 0x38}, {0x3F, 0x3F, 0x3F},
		
		{ 0x04, 0x00, 0x0a },{ 0x11, 0x17, 0x24 }, /* palette 0x20 - 0x2F    */
		{ 0x13, 0x1f, 0x06 },{ 0x2c, 0x2e, 0x2c },
		{ 0x05, 0x04, 0x11 },{ 0x18, 0x28, 0x06 },
		{ 0x22, 0x25, 0x27 },{ 0x36, 0x37, 0x35 },
		{ 0x20, 0x20, 0x1e },{ 0x0d, 0x12, 0x1d },
		{ 0x0f, 0x18, 0x06 },{ 0x29, 0x29, 0x28 },
		{ 0x1d, 0x2d, 0x0c },{ 0x18, 0x1f, 0x29 },
		{ 0x0c, 0x14, 0x04 },{ 0x15, 0x23, 0x06 },
		{ 0x31, 0x32, 0x30 },{ 0x08, 0x0b, 0x19 },
		{ 0x1e, 0x26, 0x2d },{ 0x18, 0x19, 0x1a },
		{ 0x19, 0x27, 0x0b },{ 0x12, 0x1b, 0x08 },
		{ 0x3a, 0x3b, 0x3a },{ 0x1e, 0x21, 0x26 },
		{ 0x28, 0x2b, 0x2e },{ 0x0a, 0x11, 0x02 },
		{ 0x2b, 0x2f, 0x31 },{ 0x0e, 0x15, 0x06 },
		{ 0x16, 0x26, 0x05 },{ 0x10, 0x1a, 0x05 },
		{ 0x23, 0x29, 0x2e },{ 0x1d, 0x29, 0x19 }
    };

	static unsigned char splash_RGB[64][3];
	static unsigned char background_RGB[64][3] = {
		{0x00, 0x00, 0x00}, {28, 17, 7},   /* palette 0x00 - 0x0F    */
		{20, 55, 57}, {62, 50, 3},   /* basic VGA colors       */
		{61, 34, 52}, {62, 5, 2},	
		{55, 56, 56}, {9, 27, 28}, //	Used 0-6 and 0xF
		
		
		{18, 15,  5},
		{46, 43,  26},
		{35, 26,  14},
		{34, 21,  29},
		{46, 43,  43},
		{25, 20,  22},
		{35, 33, 30},
		 { 53, 27, 13},
		 { 57, 55, 47},
		{ 59, 58, 26},
		{ 42, 33, 54},
		{ 48, 31, 28},
		
		
		
		{10,8,3},
		{46, 43, 26},
		{35, 26, 11},
		{32, 23, 28},
		{45, 43, 43},
		{25, 20, 21},
		{48, 29, 30},
		{57, 54, 47},
		{59, 59, 26},
		{53, 28, 12},
		{35, 34, 31},
		{22, 18, 6},
		
		
		
		{ 19, 15,  6},
		{ 46, 43, 26},
		{ 35, 27, 13},
		{ 33, 21, 30},
		{ 45, 43, 42},
		{ 25, 20, 21},
		{35, 33, 31},
		{ 57, 55, 48},
		{ 59, 59, 26},
		{ 53, 28, 13},
		{ 42, 33, 54},
		{ 48, 31, 28},
		
		{	 19,  15,6 },
{  46,  43,  26},
{   35,  27,  13},
{  33,  21,  30},
{  44,  43,  43},
{  24,  20,  21},
{  57,  54,  48},
{  35,  33,  30},
{  59,  59,  26},
{  53,  28,  13},
{  42,  33,  51},
{  48,  31,  28},
		
		
		// {0x15, 0x15, 0x15}, {0x15, 0x15, 0x3F},
		// {0x15, 0x3F, 0x15}, {0x15, 0x3F, 0x3F},
		// {0x3F, 0x15, 0x15}, {0x3F, 0x15, 0x3F},
		// {0x3F, 0x3F, 0x15}, {0x3F, 0x3F, 0x3F},
		// {0x00, 0x00, 0x00}, {0x05, 0x05, 0x05},   /* palette 0x10 - 0x1F    */
		// {0x08, 0x08, 0x08}, {0x0B, 0x0B, 0x0B},   /* VGA grey scale         */
		// {0x0E, 0x0E, 0x0E}, {0x11, 0x11, 0x11},
		// {0x14, 0x14, 0x14}, {0x18, 0x18, 0x18},
		// {0x1C, 0x1C, 0x1C}, {0x20, 0x20, 0x20},
		// {0x24, 0x24, 0x24}, {0x28, 0x28, 0x28},
		// {0x2D, 0x2D, 0x2D}, {0x32, 0x32, 0x32},
		// {0x38, 0x38, 0x38}, {0x3F, 0x3F, 0x3F},
		
		// { 0x04, 0x00, 0x0a },{ 0x11, 0x17, 0x24 }, /* palette 0x20 - 0x2F    */
		// { 0x13, 0x1f, 0x06 },{ 0x2c, 0x2e, 0x2c },
		// { 0x05, 0x04, 0x11 },{ 0x18, 0x28, 0x06 },
		// { 0x22, 0x25, 0x27 },{ 0x36, 0x37, 0x35 },
		// { 0x20, 0x20, 0x1e },{ 0x0d, 0x12, 0x1d },
		// { 0x0f, 0x18, 0x06 },{ 0x29, 0x29, 0x28 },
		// { 0x1d, 0x2d, 0x0c },{ 0x18, 0x1f, 0x29 },
		// { 0x0c, 0x14, 0x04 },{ 0x15, 0x23, 0x06 },
		// { 0x31, 0x32, 0x30 },{ 0x08, 0x0b, 0x19 },
		// { 0x1e, 0x26, 0x2d },{ 0x18, 0x19, 0x1a },
		// { 0x19, 0x27, 0x0b },{ 0x12, 0x1b, 0x08 },
		// { 0x3a, 0x3b, 0x3a },{ 0x1e, 0x21, 0x26 },
		
		
		// { 0x28, 0x2b, 0x2e },{ 0x0a, 0x11, 0x02 },
		// { 0x2b, 0x2f, 0x31 },{ 0x0e, 0x15, 0x06 },
		// { 0x16, 0x26, 0x05 },{ 0x10, 0x1a, 0x05 },
		// { 0x23, 0x29, 0x2e },{ 0x1d, 0x29, 0x19 }
		
		
		 {   10  ,    8    ,  3},
		{ 	 46   ,  43    , 26},
		{ 	 35 ,    26    , 11},
		{ 	 32  ,   23    , 28},
		{ 	 45  ,   43    , 43},
		{ 	 25  ,   20   ,  21},
		{ 	 48  ,   29  ,   30},
		{ 	 57  ,   54  ,   47}
		
		
		
	};
	
	
	
	static unsigned char background_palette1[64][3] = {
		{ 	 59  ,   59  ,   26},
		{ 	 53  ,   28  ,   12},
		{ 	 35  ,   34  ,   31},
		{ 	 22  ,   18 ,     6},
		
		{0x00, 0x00, 0x00}, {0x05, 0x05, 0x05},   /* palette 0x10 - 0x1F    */
		{0x08, 0x08, 0x08}, {0x0B, 0x0B, 0x0B},   /* VGA grey scale         */
		{0x0E, 0x0E, 0x0E}, {0x11, 0x11, 0x11},
		{0x14, 0x14, 0x14}, {0x18, 0x18, 0x18},
		
		{38	,28	,21},
		{54	,55	,47},
		{36	,43	,46},
		{56	,61	,61},
		{39	,40	,31},
		{53	,49	,46},
		{53	,42	,26},
		{53	,56	,58},
		{42	,50	,55},
		{62	,62	,62},
		{43	,47	,46},
		{54	,46	,35},
		
		
		
		{0x1C, 0x1C, 0x1C}, {0x20, 0x20, 0x20},
		{0x24, 0x24, 0x24}, {0x28, 0x28, 0x28},
		{0x2D, 0x2D, 0x2D}, {0x32, 0x32, 0x32},
		{0x38, 0x38, 0x38}, {0x3F, 0x3F, 0x3F},
		
		{ 0x04, 0x00, 0x0a },{ 0x11, 0x17, 0x24 }, /* palette 0x20 - 0x2F    */
		{ 0x13, 0x1f, 0x06 },{ 0x2c, 0x2e, 0x2c },
		{ 0x05, 0x04, 0x11 },{ 0x18, 0x28, 0x06 },
		{ 0x22, 0x25, 0x27 },{ 0x36, 0x37, 0x35 },
		{ 0x20, 0x20, 0x1e },{ 0x0d, 0x12, 0x1d },
		{ 0x0f, 0x18, 0x06 },{ 0x29, 0x29, 0x28 },
		{ 0x1d, 0x2d, 0x0c },{ 0x18, 0x1f, 0x29 },
		{ 0x0c, 0x14, 0x04 },{ 0x15, 0x23, 0x06 },
		{ 0x31, 0x32, 0x30 },{ 0x08, 0x0b, 0x19 },
		{ 0x1e, 0x26, 0x2d },{ 0x18, 0x19, 0x1a },
		{ 0x19, 0x27, 0x0b },{ 0x12, 0x1b, 0x08 },
		{ 0x3a, 0x3b, 0x3a },{ 0x1e, 0x21, 0x26 },
		{ 0x28, 0x2b, 0x2e },{ 0x0a, 0x11, 0x02 },
		{ 0x2b, 0x2f, 0x31 },{ 0x0e, 0x15, 0x06 },
		{ 0x16, 0x26, 0x05 },{ 0x10, 0x1a, 0x05 },
		{ 0x23, 0x29, 0x2e },{ 0x1d, 0x29, 0x19 }
    };
	
	int i,r=0,g=0,b=0;
	for(i=0;i<32;i++) {
		splash_RGB[0x0 + i][0] = r;
		splash_RGB[0x0 + i][1] = g;
		splash_RGB[0x0 + i][2] = b;
		r+=2;
		g+=2;
		b+=2;
	}
	
	for(i=32;i<64;i++) {
		r = palette_RGB[i][0];
		g = palette_RGB[i][1];
		b = palette_RGB[i][2];
		splash_RGB[i][0] = (0xD);
		splash_RGB[i][1] = (0x1B);
		splash_RGB[i][2] = (0x3F);

	}
    /* Start writing at color 0. */
    OUTB (0x03C8, 0x00);

    /* Write all 64 colors from array. */
    REP_OUTSB (0x03C9, palette_RGB, 64 * 3);
    REP_OUTSB (0x03C9, splash_RGB, 64 * 3);
    REP_OUTSB (0x03C9, background_RGB, 64 * 3);
    REP_OUTSB (0x03C9, background_palette1, 64 * 3);
	

}

/*
 * clear_screens
 *   DESCRIPTION: Fills the video memory with zeroes. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: fills all 256kB of VGA video memory with zeroes
 */   
void clear_screens ()
{
    /* Write to all four planes at once. */ 
    SET_WRITE_MASK (0x0F00);

    /* Set 64kB to zero (times four planes = 256kB). */
    memset (mem_image, 0, MODE_X_MEM_SIZE);
}


/*
 * set_text_mode_3
 *   DESCRIPTION: Put VGA into text mode 3 (color text).
 *   INPUTS: clear_scr -- if non-zero, clear screens; otherwise, do not
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: may clear screens; writes font data to video memory
 */   
void set_text_mode_3 (int clear_scr)
{
    unsigned long* txt_scr; /* pointer to text screens in video memory */
    int i;                  /* loop over text screen words             */
	
    VGA_blank (1);                               /* blank the screen        */
    /* 
     * The value here had been changed to 0x63, but seems to work
     * fine in QEMU (and VirtualPC, where I got it) with the 0x04
     * bit set (VGA_MIS_DCLK_28322_720).  
     */
    set_seq_regs_and_reset (text_seq, 0x67);     /* sequencer registers     */
    set_CRTC_registers (text_CRTC);              /* CRT control registers   */
    set_attr_registers (text_attr);              /* attribute registers     */
    set_graphics_registers (text_graphics);      /* graphics registers      */
    fill_palette ();				 /* palette colors          */
    if (clear_scr) {				 /* clear screens if needed */
	txt_scr = (unsigned long*)(mem_image + 0x18000); 
	for (i = 0; i < 8192; i++)
	    *txt_scr++ = 0x07200720;
    }
    write_font_data ();  			/* copy fonts to video mem */
	VGA_blank (0);			         /* unblank the screen      */
	mem_image = (void *)0x10000;
	isModex = 0;
}

/*
 * write_font_data
 *   DESCRIPTION: Copy font data into VGA memory, changing and restoring
 *                VGA register values in order to do so. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: leaves VGA registers in final text mode state
 */   
void write_font_data ()
{
    int i;                /* loop index over characters                   */
    int j;                /* loop index over font bytes within characters */
    unsigned char* fonts; /* pointer into video memory                    */

    /* Prepare VGA to write font data into video memory. */
    OUTW (0x3C4, 0x0402);
    OUTW (0x3C4, 0x0704);
    OUTW (0x3CE, 0x0005);
    OUTW (0x3CE, 0x0406);
    OUTW (0x3CE, 0x0204);
	OUTB (0X03C8,0X22);

    /* Copy font data from array into video memory. */
    for (i = 0, fonts = mem_image; i < 256; i++) {
	for (j = 0; j < 16; j++)
	    fonts[j] = (unsigned char)font_data[i][j];
	fonts += 32; /* skip 16 bytes between characters */
    }

    /* Prepare VGA for text mode. */
    OUTW (0x3C4, 0x0302);
    OUTW (0x3C4, 0x0304);
    OUTW (0x3CE, 0x1005);
    OUTW (0x3CE, 0x0E06);
    OUTW (0x3CE, 0x0004);
}



void
copy_image (unsigned char* img )
{
    /* 
     * memcpy is actually probably good enough here, and is usually
     * implemented using ISA-specific features like those below,
     * but the code here provides an example of x86 string moves
     */
    asm volatile (
        "cld                                                 ;"
       	"movl $16000,%%ecx                                   ;"
       	"rep movsb    # copy ECX bytes from M[ESI] to M[EDI]  "
      : /* no outputs */
      : "S" (img), "D" (mem_image) 
      : "eax", "ecx", "memory"
    );
}



/*
 * draw_full_block
 *   DESCRIPTION: Draw a BLOCK_X_DIM x BLOCK_Y_DIM block at absolute 
 *                coordinates.  Mask any portion of the block not inside 
 *                the logical view window.
 *   INPUTS: (pos_x,pos_y) -- coordinates of upper left corner of block
 *           blk -- image data for block (one byte per pixel, as a C array
 *                  of dimensions [BLOCK_Y_DIM][BLOCK_X_DIM])
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: draws into the build buffer
 */   
void
draw_full_block (int pos_x, int pos_y, unsigned char* blk, uint32_t block_x_dim,uint32_t block_y_dim )
{
    int dx, dy;          /* loop indices for x and y traversal of block */
    int x_left, x_right; /* clipping limits in horizontal dimension     */
    int y_top, y_bottom; /* clipping limits in vertical dimension       */

	  /* If block is completely off-screen, we do nothing. */
    if (pos_x + block_x_dim <= show_x || pos_x >= show_x + SCROLL_X_DIM ||
        pos_y + block_y_dim <= show_y || pos_y >= show_y + SCROLL_Y_DIM)
	return;
   
    /* Clip any pixels falling off the left side of screen. */
    if ((x_left = show_x - pos_x) < 0)
        x_left = 0;
    /* Clip any pixels falling off the right side of screen. */
    if ((x_right = show_x + SCROLL_X_DIM - pos_x) > block_x_dim)
        x_right = block_x_dim;
    /* Skip the first x_left pixels in both screen position and block data. */
    pos_x += x_left;
    blk += x_left;
	
    /* 
     * Adjust x_right to hold the number of pixels to be drawn, and x_left
     * to hold the amount to skip between rows in the block, which is the
     * sum of the original left clip and (BLOCK_X_DIM - the original right 
     * clip).
     */
    x_right -= x_left;
    x_left = block_x_dim - x_right; 

    /* Clip any pixels falling off the top of the screen. */
    if ((y_top = show_y - pos_y) < 0)
        y_top = 0;
    /* Clip any pixels falling off the bottom of the screen. */
    if ((y_bottom = show_y + SCROLL_Y_DIM - pos_y) > block_y_dim)
        y_bottom = block_y_dim;
    /* 
     * Skip the first y_left pixel in screen position and the first
     * y_left rows of pixels in the block data.
     */
    pos_y += y_top;
    blk += y_top * block_x_dim;
    /* Adjust y_bottom to hold the number of pixel rows to be drawn. */
    y_bottom -= y_top;

    /* Draw the clipped image. */
    for (dy = 0; dy < y_bottom; dy++, pos_y++) {
		for (dx = 0; dx < x_right; dx++, pos_x++, blk++){
				if(*blk!=0) 
					*(img3 + (pos_x >> 2) + pos_y * SCROLL_X_WIDTH + (3 - (pos_x & 3)) * SCROLL_SIZE) = *blk;
			
		}
	pos_x -= x_right;
	blk += x_left;
	}
}


void
save_full_block (int pos_x, int pos_y, unsigned char* blk, uint32_t block_x_dim,uint32_t block_y_dim )
{
    int dx, dy;          /* loop indices for x and y traversal of block */
    int x_left, x_right; /* clipping limits in horizontal dimension     */
    int y_top, y_bottom; /* clipping limits in vertical dimension       */

	  /* If block is completely off-screen, we do nothing. */
    if (pos_x + block_x_dim <= show_x || pos_x >= show_x + SCROLL_X_DIM ||
        pos_y + block_y_dim <= show_y || pos_y >= show_y + SCROLL_Y_DIM)
	return;
   
    /* Clip any pixels falling off the left side of screen. */
    if ((x_left = show_x - pos_x) < 0)
        x_left = 0;
    /* Clip any pixels falling off the right side of screen. */
    if ((x_right = show_x + SCROLL_X_DIM - pos_x) > block_x_dim)
        x_right = block_x_dim;
    /* Skip the first x_left pixels in both screen position and block data. */
    pos_x += x_left;
    blk += x_left;
	
    /* 
     * Adjust x_right to hold the number of pixels to be drawn, and x_left
     * to hold the amount to skip between rows in the block, which is the
     * sum of the original left clip and (BLOCK_X_DIM - the original right 
     * clip).
     */
    x_right -= x_left;
    x_left = block_x_dim - x_right; 

    /* Clip any pixels falling off the top of the screen. */
    if ((y_top = show_y - pos_y) < 0)
        y_top = 0;
    /* Clip any pixels falling off the bottom of the screen. */
    if ((y_bottom = show_y + SCROLL_Y_DIM - pos_y) > block_y_dim)
        y_bottom = block_y_dim;
    /* 
     * Skip the first y_left pixel in screen position and the first
     * y_left rows of pixels in the block data.
     */
    pos_y += y_top;
    blk += y_top * block_x_dim;
    /* Adjust y_bottom to hold the number of pixel rows to be drawn. */
    y_bottom -= y_top;

    /* Draw the clipped image. */
    for (dy = 0; dy < y_bottom; dy++, pos_y++) {
		for (dx = 0; dx < x_right; dx++, pos_x++, blk++){
			if(*blk!=0)
				 *blk = *(img3 + (pos_x >> 2) + pos_y * SCROLL_X_WIDTH + (3 - (pos_x & 3)) * SCROLL_SIZE);
		}
	pos_x -= x_right;
	blk += x_left;
	}
}


void draw_background(){
	int i;
	// uint8_t buf[320*200];
	// for (i = 0; i < 320*200;i ++){
		// buf[i] = background_img[i];
	// }
	draw_full_block(0,0, head_img.matrix_ptr, 320,200);
	show_screen();
}




void
draw_save_full_block ( unsigned char* blk, unsigned char *store_buf,int pos_x, int pos_y, uint32_t block_x_dim,uint32_t block_y_dim, uint8_t needSave )
{
    int dx, dy;          /* loop indices for x and y traversal of block */
    int x_left, x_right; /* clipping limits in horizontal dimension     */
    int y_top, y_bottom; /* clipping limits in vertical dimension       */

	  /* If block is completely off-screen, we do nothing. */
    if (pos_x + block_x_dim <= show_x || pos_x >= show_x + SCROLL_X_DIM ||
        pos_y + block_y_dim <= show_y || pos_y >= show_y + SCROLL_Y_DIM)
	return;
   
    /* Clip any pixels falling off the left side of screen. */
    if ((x_left = show_x - pos_x) < 0)
        x_left = 0;
    /* Clip any pixels falling off the right side of screen. */
    if ((x_right = show_x + SCROLL_X_DIM - pos_x) > block_x_dim)
        x_right = block_x_dim;
    /* Skip the first x_left pixels in both screen position and block data. */
    pos_x += x_left;
    blk += x_left;
	store_buf += x_left;
    /* 
     * Adjust x_right to hold the number of pixels to be drawn, and x_left
     * to hold the amount to skip between rows in the block, which is the
     * sum of the original left clip and (BLOCK_X_DIM - the original right 
     * clip).
     */
    x_right -= x_left;
    x_left = block_x_dim - x_right; 

    /* Clip any pixels falling off the top of the screen. */
    if ((y_top = show_y - pos_y) < 0)
        y_top = 0;
    /* Clip any pixels falling off the bottom of the screen. */
    if ((y_bottom = show_y + SCROLL_Y_DIM - pos_y) > block_y_dim)
        y_bottom = block_y_dim;
    /* 
     * Skip the first y_left pixel in screen position and the first
     * y_left rows of pixels in the block data.
     */
    pos_y += y_top;
    blk += y_top * block_x_dim;
    store_buf += y_top * block_x_dim;
    /* Adjust y_bottom to hold the number of pixel rows to be drawn. */
    y_bottom -= y_top;

    /* Draw the clipped image. */

	if(needSave == 1){
		for (dy = 0; dy < y_bottom; dy++, pos_y++) {
			for (dx = 0; dx < x_right; dx++, pos_x++, blk++, store_buf ++){
					if(*blk!=0) {
						*store_buf = *(img3 + (pos_x >> 2) + pos_y * SCROLL_X_WIDTH + (3 - (pos_x & 3)) * SCROLL_SIZE) ;
						*(img3 + (pos_x >> 2) + pos_y * SCROLL_X_WIDTH + (3 - (pos_x & 3)) * SCROLL_SIZE) = *blk;
					}
			}
		pos_x -= x_right;
		blk += x_left;
		store_buf += x_left;
		}
	}
	else {
		for (dy = 0; dy < y_bottom; dy++, pos_y++) {
			for (dx = 0; dx < x_right; dx++, pos_x++, blk++){
					if(*blk!=0) {
						*(img3 + (pos_x >> 2) + pos_y * SCROLL_X_WIDTH + (3 - (pos_x & 3)) * SCROLL_SIZE) = *blk;
					}
			}
		pos_x -= x_right;
		blk += x_left;
		store_buf += x_left;
		}
	
	}
}

// build_pacman[64];


// void
// draw_save_full_block_pacman ( unsigned char* blk, unsigned char *store_buf , uint32_t block_x_dim,uint32_t block_y_dim, uint8_t needSave )
// {
	// int x, y;
	// int plane_size = 16;
	// int offset;
	// if(needSave == 0){
		// for(y = 0 ; y < block_y_dim; y ++){
			// for(x = 0 ; x < block_x_dim; x ++){
				// offset = x/4 + y*2;
				// if(blk[y*block_x_dim + x] != 0)
					// build_pacman[plane_size*(x%4) + offset] = blk[y*block_x_dim + x];
			// }
		// }
	// }
	// else{
		// for(y = 0 ; y < block_y_dim; y ++){
			// for(x = 0 ; x < block_x_dim; x ++){
				// offset = x/4 + y*2;
				// if(blk[y*block_x_dim + x] != 0){
					// store_buf[y*block_x_dim + x] = build_pacman[plane_size*(x%4) + offset];
					// build_pacman[plane_size*(x%4) + offset] = blk[y*block_x_dim + x];
				// }
			// }
		// }	
	// }
// }


void show_small_block(unsigned char *blk, int x_pos, int y_pos){
	int i;
	// int x_pos = 20;
	// int y_pos = 20;
	// unsigned char blk[8*8];
	// for (i = 0 ; i < 64; i ++){
		// blk[i] = 0xF;
	// }
	int x,y;

	int p_off = x_pos %4;
	int y_block_dim = 8;
	SET_WRITE_MASK(p_off);
	for(i =0  ; i <4; i++){
		// SET_WRITE_MASK (1 << (i + 8));
		SET_WRITE_MASK (1 << ((p_off + i )%4+ 8));
		x = x_pos + i;
		for(y= 0 ; y < y_block_dim; y ++ ){
			if( blk[i + 8* y]!= 0)
				mem_image[x/4 + (y+ y_pos)*80]  = blk[i + 8* y];
			if( blk[i +4+ 8* y]!= 0)
			mem_image[x/4 + (y+ y_pos)*80 +1]  = blk[i+4 + 8* y];
		}	
	}
}


