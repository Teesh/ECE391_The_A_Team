#ifndef SOUND_H
#define SOUND_H

#include "lib.h"
#include "paging.h"
#include "file.h"
#include "x86_desc.h"
#include "rtc.h"
#include "terminal.h"
#include "i8259.h"
#include "pit.h"

#define Fs5 740
#define Cs5 554
#define B4	494
#define Fs4 370
#define As4 466
#define E5	659
#define Ds5 622
#define Gs5 831

#define T0_16 0
#define Tq_16 12
#define Th_16 25
#define T1_16 50
#define T2_16 (50*2)
#define T3_16 (50*3)
#define T4_16 (50*4)
#define T5_16 (50*5)
#define T6_16 (50*6)
#define T7_16 (50*7)
#define T8_16 (50*8)
#define T9_16 (50*9)
#define T10_16 (50*10)
#define T11_16 (50*11)
#define T12_16 (50*12)
#define T13_16 (50*13)
#define T14_16 (50*14)
#define T15_16 (50*15)
#define T16_16 (50*16)

extern void play_sound(uint32_t nFrequence) ;
extern void nosound() ;
void beep(uint32_t freq,int time,int wait);
void ateam(int time);

#endif 
