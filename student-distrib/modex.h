/*									tab:8
 *
 * modex.h - header file for mode X 320x200 graphics
 *
 * "Copyright (c) 2004-2009 by Steven S. Lumetta."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO 
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE 
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE, 
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:	    Steve Lumetta
 * Version:	    2
 * Creation Date:   Thu Sep  9 23:08:21 2004
 * Filename:	    modex.h
 * History:
 *	SL	1	Thu Sep  9 23:08:21 2004
 *		First written.
 *	SL	2	Sat Sep 12 13:35:41 2009
 *		Integrated original release back into main code base.
 */

#ifndef MODEX_H
#define MODEX_H


#include "text.h"
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "idt.h"
#include "rtc.h"
#include "file.h"
#include "paging.h"
#include "terminal.h"
#include "system.h"
#include "text.h"
#include "sound.h"

/* 
 * IMAGE  is the whole screen in mode X: 320x200 pixels in our flavor.
 * SCROLL is the scrolling region of the screen.
 *
 * X_DIM   is a horizontal screen dimension in pixels.
 * X_WIDTH is a horizontal screen dimension in 'natural' units
 *         (addresses, characters of text, etc.)
 * Y_DIM   is a vertical screen dimension in pixels.
 */
#define IMAGE_X_DIM     320   /* pixels; must be divisible by 4             */
#define SCROLL_HEIGHT   18
#define SCROLL_BAR       (IMAGE_X_DIM*SCROLL_HEIGHT)
#define IMAGE_Y_DIM     200   /* pixels                                     */
#define IMAGE_X_WIDTH   (IMAGE_X_DIM / 4)          /* addresses (bytes)     */
#define SCROLL_X_DIM	IMAGE_X_DIM                /* full image width      */
#define SCROLL_Y_DIM    IMAGE_Y_DIM                /* full image width      */
#define SCROLL_X_WIDTH  (IMAGE_X_DIM / 4)          /* addresses (bytes)     */

#define SCROLL_SIZE     (SCROLL_X_WIDTH * SCROLL_Y_DIM)
#define SCREEN_SIZE	(SCROLL_SIZE * 4 + 1)
#define BUILD_BUF_SIZE  (SCREEN_SIZE)// + 20000) 
// #define BUILD_BASE_INIT ((BUILD_BUF_SIZE - SCREEN_SIZE) / 2)
#define BLOCK_X_DIM 20
#define BLOCK_Y_DIM 20


#define BLACK	3;
#define SILVER  1;
#define GREEN  2;


#define VID_MEM_SIZE       131072
#define MODE_X_MEM_SIZE     65536
#define NUM_SEQUENCER_REGS      5
#define NUM_CRTC_REGS          25
#define NUM_GRAPHICS_REGS       9
#define NUM_ATTR_REGS          22




extern int set_mode_X ();
extern void clear_mode_X ();
extern void clear_screens ();
void VGA_blank (int blank_bit);
void set_seq_regs_and_reset (unsigned short table[NUM_SEQUENCER_REGS], unsigned char val);
void set_CRTC_registers (unsigned short table[NUM_CRTC_REGS]);
void set_attr_registers (unsigned char table[NUM_ATTR_REGS * 2]);
void set_graphics_registers (unsigned short table[NUM_GRAPHICS_REGS]);
//
extern int set_modex();
extern void start_splash();
extern void start_gui() ;
extern void show_screen ();
extern void copy_image (unsigned char* img );
extern void add_img();
extern void background(void);
extern void start_gif(void);
extern void start_paint(void);

extern int32_t mouse_x;
extern int32_t mouse_y;
extern uint8_t isModex;
extern uint8_t isPacman;
extern unsigned char* build;

extern int32_t last_key;
extern int32_t isPlaying;
//
void fill_palette ();

void clear_screens ();

extern void set_text_mode_3 (int clear_scr);

void write_font_data ();

extern void draw_background();
void start_modex(void);
void draw_lose_pacman();
void draw_win_pacman();


void draw_full_block (int pos_x, int pos_y, unsigned char* blk, uint32_t block_x_dim,uint32_t block_y_dim );
void save_full_block (int pos_x, int pos_y, unsigned char* blk, uint32_t block_x_dim,uint32_t block_y_dim );
extern void init_modex(void);
extern void bring_to_front(int id);
extern void move_window(int id, int32_t del_x, int32_t del_y);
extern void close_window(int id);
extern void clock(uint32_t sec);
extern void paint_background();
extern void clear_paint();

typedef struct image {
	uint32_t id;
	uint8_t * matrix_ptr;	//1
	uint32_t height;		//4
	uint32_t width;			//4
	int32_t start_x;			//4
	int32_t start_y;		//4
	uint32_t priority_num;//4
	struct image * next;	//4
	struct image * prev;	//4
} image_t;

typedef struct pacman_st {
	int32_t pos_x;
	int32_t pos_x_in;
	int32_t pos_y;
	int32_t pos_y_in;
	int32_t step_x;
	int32_t step_x_in;
	int32_t step_y;
	int32_t step_y_in;
	int32_t direction;
	int32_t direction_in;
	int32_t maze_x;
	int32_t maze_y;
} pacman_t;

extern image_t* head_img_ptr;
extern image_t* tail_img_ptr;
extern unsigned char * build_ptr;
extern uint8_t paint_color;
extern uint8_t paint_size;
extern uint32_t parse_pacman(void * pacman_in, int32_t flag);

void draw_save_full_block ( unsigned char* blk, unsigned char *store_buf,int pos_x, int pos_y, uint32_t block_x_dim,uint32_t block_y_dim, uint8_t needSave );
extern void load_maze();
void find_pacman_next_position();
uint8_t canMove(int x, int y, int dir);
void start_pacman();
extern void init_pacman();
void show_small_block(unsigned char *blk, int x_pos, int y_pos);
void draw_paint(uint32_t x,uint32_t y);

#endif /* MODEX_H */


