#ifndef TEXT_H
#define TEXT_H

#include "lib.h"
#include "paging.h"
#include "file.h"
#include "x86_desc.h"
#include "rtc.h"
#include "terminal.h"
#include "i8259.h"
#include "pit.h"

/* The default VGA text mode font is 8x16 pixels. */
#define FONT_WIDTH   8
#define FONT_HEIGHT 16
#define TEXT_BLK_X_DIM 96
#define TEXT_BLK_Y_DIM 16

/* Standard VGA text font. */
extern uint8_t font_data[256][16];
extern unsigned char pacman_block[8*8];
extern unsigned char pacman_block_right[8*8];
extern unsigned char pacman_block_left[8*8];
extern unsigned char pacman_block_up[8*8];
extern unsigned char pacman_block_down[8*8];
extern unsigned char ghost_block[8*8] ;

extern unsigned char wall_0_block[8*8];
extern unsigned char wall_1_block[8*8];
extern unsigned char wall_2_block[8*8];
extern unsigned char wall_3_block[8*8];
extern unsigned char wall_4_block[8*8];
extern unsigned char wall_5_block[8*8];
extern unsigned char wall_6_block[8*8];
extern unsigned char wall_7_block[8*8];

extern unsigned char * frames[5];

extern uint8_t * background_img;
extern uint8_t * status_bar;
extern uint8_t * timer_block;
extern uint8_t * terminal_img;
extern uint8_t * mouse_img;
extern uint8_t * folder_img;
extern uint8_t * window_img;
extern uint8_t * pacman_img;
extern uint8_t * paint_img;
extern uint8_t * paint_window;
extern unsigned char * pacman_fail;
extern unsigned char * file_img;
extern uint8_t * gif_viewer;

extern void make_text(unsigned char * buf,unsigned int size,char * input);
extern void create_block_text(unsigned char *buff, unsigned char *str);
extern void create_block_text_double(unsigned char *buff, char *str);
extern void text_init(void);
void create_block_text_timer(unsigned char *buff, char *str);
void create_block_text_name(unsigned char *buff, char *str, int length);

#endif /* TEXT_H */
