#ifndef _PIT_H
#define _PIT_H

#include "lib.h"
#include "paging.h"
#include "file.h"
#include "x86_desc.h"
#include "rtc.h"
#include "terminal.h"
#include "i8259.h"
#include "types.h"

#define PIT_IRQ 0

void init_PIT(uint32_t freq);
void pit_irq(void);

extern void PIT_IRQ_HANDLER();

#endif /* _PIT_H */


