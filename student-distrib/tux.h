#ifndef _TUX_H
#define _TUX_H

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

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8
#define TUX_PORT COM1

extern void serial_init();

int serial_received() ;
char read_serial() ;
int is_transmit_empty() ;
void write_serial(char a) ;


#endif
