#ifndef _MOUSE_H
#define _MOUSE_H

#include "lib.h"
#include "paging.h"
#include "file.h"
#include "x86_desc.h"
#include "rtc.h"
#include "terminal.h"
#include "i8259.h"
#include "pit.h"
#include "idt.h"
#include "modex.h"

#define MOUSE_IRQ 12
#define MOUSE_COM_PORT 0x64
#define MOUSE_DATA_PORT 0x60

void mouse_init(void);
void mouse_handler(void);
extern void mouse_write(uint8_t write_char);
extern uint8_t mouse_read(void);
void draw_mouse(int32_t del_x,int32_t del_y);

extern uint32_t left_drag_flag;

#endif /* _MOUSE_H */

