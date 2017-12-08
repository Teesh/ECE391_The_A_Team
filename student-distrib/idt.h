#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "key.h"
#include "terminal.h"
#include "system.h"
#include "pit.h"
#include "mouse.h"

#define RTC_IDT 0x28
#define KEY_IDT 0x21
#define INT80_IDT 0x80
#define PIT_IDT 0x20
#define MOUSE_IDT 0x2C

// IDT initializer
void load_idt();
// interrupt 0 - divide error (DE)
void exception_DE();
// interrupt 1 - debug (DB)
void exception_DB();
// interrupt 2 - NMI interrupt
void exception_NMI();
// interrupt 3 - Breakpoint (BP)
void exception_BP();
// interrupt 4 - Overflow (OF)
void exception_OF();
// interrupt 5 - BOUND Range Exceeded (BR)
void exception_BR();
// interrupt 6 - Invalid Opcode (UD)
void exception_UD();
// interrupt 7 - Device Not Available (NM)
void exception_NM();
// interrupt 8 - Double Fault (DF)
void exception_DF();
// interrupt 9 - Coprocessor Segment Overrun
void exception_CSO();
// interrupt 10 - Invalid TSS (TS)
void exception_TS();
// interrupt 11 - Segment Not Present (NP)
void exception_NP();
// interrupt 12 - Stack Fault (SS)
void exception_SS();
// interrupt 13 - General Protection (GP)
void exception_GP();
// interrupt 14 - Page-Fault (PF)
void exception_PF();
// interrupt 16 - x87 FPU Floating-Point Error (MF)
void exception_MF();
// interrupt 17 - Alignment Check (AC)
void exception_AC();
// interrupt 18 - Machine-Check (MC)
void exception_MC();
// interrupt 19 - SIMD Floating-Point (XF)
void exception_XF();

#endif /* _IDT_H */

