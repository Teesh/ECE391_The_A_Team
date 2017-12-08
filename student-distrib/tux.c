#include "tux.h"

void serial_init(){
	outb(0x00,TUX_PORT + 1);    // Disable all interrupts
	outb(0x80,TUX_PORT + 3);    // Enable DLAB (set baud rate divisor)
	outb(0x0C,TUX_PORT + 0);    // Set divisor to 12 (lo byte) 9600 baud
	outb(0x00,TUX_PORT + 1);    //                  (hi byte)
	outb(0x03,TUX_PORT + 3);    // 8 bits, no parity, one stop bit
	outb(0x03,TUX_PORT + 2);    // Enable FIFO, clear them, with 14-byte threshold
	outb(0x0B,TUX_PORT + 4);    // IRQs enabled, RTS/DSR set
}


int serial_received() {
   return inb(TUX_PORT + 5) & 1;
}

char read_serial() {
	int i=0;
	char buf;
	while(1) {
		if(serial_received() != 0) break;
		rtc_read(-1,&buf,1);
		i++;
		if(i>1024*10) {
			return 0xFF;
		}
	}
	return inb(TUX_PORT);
}

int is_transmit_empty() {
   return inb(TUX_PORT + 5) & 0x20;
}
 
void write_serial(char a) {
   while (is_transmit_empty() == 0);
   outb(a,TUX_PORT);
}

