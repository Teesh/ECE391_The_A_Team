#include "rtc.h"



/*
	rtc_init - initialize RTC
	inputs - none
	outputs - none
	side effects - RTC initialized
*/
void rtc_init() {
	//cli();
	
	char save;
	outb(RTC_Reg_B,RTC_Port1);
	save = inb(RTC_Port2);
	outb(RTC_Reg_B,RTC_Port1);
	outb(save|RTC_Bit_6,RTC_Port2);

	rtc_ops.open = &rtc_open;
	rtc_ops.read = &rtc_read;
	rtc_ops.write = &rtc_write;
	rtc_ops.close = &rtc_close;
	//sti();
}
static char rtc_readf[3]={0,0,0};//flag for if rtc has been read

/*

	rtc_handler - Handle RTC interrupts
	inputs - none
	outputs - none
	side effects - none, unless test_interrupts uncommented, then rave party
	
*/
void rtc_handler() {
	cli();
	// test_interrupts();
	rtc_readf[term_pit]=0;
	// Register C needs to be thrown away after every IRQ8
	outb(RTC_Reg_C,RTC_Port1);
	inb(RTC_Port2);

	// EOI needs to be sent after every IRQ
	send_eoi(RTC_IRQ);
	sti();
}

static int rtc_table[10] = {2,4,8,16,32,64,128,256,512,1024};
/*
	rtc_write - Handles writing to RTC
	inputs - file descriptor, buffer, number of bytes to be read
	outputs - none
	side effects - change rate of speed of interrupts
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
	
	const int * rate=(int *)buf;
	int i,val,flag=0;
	for(i=0;i<10;i++) {
		if(*rate == rtc_table[i]) {
			flag = 1;
			val = 15 - i;
			break;
		}
	}
	if(!flag) return -1;
	// rate &= 0x0F;			// rate must be above 2 and not over 15
	cli();
	outb(RTC_Reg_A, RTC_Port1);		// set index to register A, disable NMI
	char prev=inb(RTC_Port2);	// get initial value of register A
	outb(RTC_Reg_A,RTC_Port1);		// reset index to A
	char newr=(prev & 0xF0) | val;//get new rate
	outb(newr , RTC_Port2); //write only our rate to A. Note, rate is the bottom 4 bits.
	sti();
	return 0;
}

/*
	rtc_open - nothing yet
	inputs - filename
	outputs - none
	side effects - nothing yet
*/

int32_t rtc_open(const uint8_t* filename){//supposed to be like rtc init...?
	return 0;
}

/*
	rtc_read - Handles reading RTC
	inputs - file descriptor, buffer, number of bytes to be read
	outputs - none
	side effects - reads that interrupt happens
*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
	cli();
	rtc_readf[term_pit]=1;//set flag to wait for handler to clear
	sti();
	while(1) 
	{
		cli();
		if(rtc_readf[term_pit]==0) {
			sti();
			return 0;//return once handler clears 
		}
		sti();
	}
}

/*
	rtc_close - nothing yet
	inputs - filedescriptor
	outputs - none
	side effects - nothing yet
*/

int32_t rtc_close(int32_t fd){
	return 0;
}
