#ifndef _RTC_H
#define _RTC_H

#define RTC_Port1 0x70
#define RTC_Port2 0x71

#define RTC_Reg_A 0x8A
#define RTC_Reg_B 0x8B
#define RTC_Reg_C 0x0C
#define RTC_Bit_6 0x40
#define RTC_IDT_ENTRY 0x28
#define RTC_IRQ 8
#define MIN_RTC_RATE 3
#define MAX_RTC_RATE 15

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "system.h"

// rtc init and handler functions
void rtc_init(void);
void rtc_handler(void);
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_close(int32_t fd);



#endif /* _RTC_H */
