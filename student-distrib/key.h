#ifndef _KEY_H
#define _KEY_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "system.h"
#include "paging.h"
#include "pit.h"
#include "modex.h"
#include "mouse.h"

#define KEY_PORT 0x60
#define KEY_IRQ 1

#define RESET 		0xff
#define ENABLE 		0xf4
#define SCAN_ONE 	0xf1
#define KEY_ACK 	0xfa
#define TEST_DONE 	0xaa

#define STEP  2
#define STOP 0
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_RIGHT 3
#define KEY_LEFT 4

#define LEFT_SHIFT_DOWN		0x2A
#define RIGHT_SHIFT_DOWN 	0x36
#define LEFT_SHIFT_UP		0xAA
#define RIGHT_SHIFT_UP		0xB6
#define LEFT_CTRL_DOWN		0x1D
#define LEFT_CTRL_UP		0x9D
#define CAPS_LOCK			0x3A
#define LEFT_ALT_DOWN		0x38
#define LEFT_ALT_UP			0xB8
#define L_KEY				0x26
#define DELETE				0x8E
#define TAB					0X0F
#define ESC					0x01
#define F1					0x3B
#define F2					0x3C
#define F3					0x3D
#define C_KEY				0x2E
#define UP					0x48
#define LEFT				0x4B
#define RIGHT				0x4D
#define DOWN				0x50
#define INSERT				0x52

#define ONE 0x02 
#define TWO 0x03 
#define THREE 0x04
#define FOUR 0x05 
#define FIVE 0x06 
#define SIX 0x07 
#define SEVEN 0x08 
#define EIGHT 0x09 
#define NINE 0x0A 
#define ZERO 0x0B 

#define SIZE_ONE 0x10
#define SIZE_TWO 0x11
#define SIZE_THREE 0x12
#define SIZE_FOUR 0x13
#define SIZE_FIVE 0x14
#define SIZE_SIX 0x15


void key_init();
void key_handler();

#endif /* _KEY_H */
