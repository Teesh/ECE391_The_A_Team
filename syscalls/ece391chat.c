#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"
#define BUFSIZE 1024
int main ()
{
	uint32_t i, turn=0;
	int32_t cnt;
	uint8_t buf[1024];
	uint8_t buf2[1024];
	uint8_t buf3[1024];
	uint8_t buf4[1024];
	uint8_t c;
	uint32_t un_flag,com_flag;
	int garbage;
    int rtc_fd;
	
	ece391_fdputs (1, (uint8_t*)"Hi, what's your name? ");
    if (-1 == (cnt = ece391_read (0, buf, BUFSIZE-1))) {
        ece391_fdputs (1, (uint8_t*)"Can't read name from keyboard.\n");
        return 3;
    }
	if (cnt > 0 && '\n' == buf[cnt - 1])
	    cnt--;
	buf[cnt] = '\0';
	
	ece391_tx(buf[0]);
	c = ece391_rx();
	if(c == 0xFF) {	
		ece391_fdputs (1, (uint8_t*)"Connection timed out\n");
		return 3;
	}
	if(c > buf[0]) turn = 1;
	else turn = 0;
	
	un_flag = 0;
	com_flag = 0;
	while(com_flag < 2) {
		if(turn == 1) {
			for(i=0;buf[i] != 0 &&  buf[i] != '\n';i++) {
				ece391_tx(buf[i]);
				c = ece391_rx();
				if(c!=0xFA) {
					ece391_fdputs (1, (uint8_t*)"Transmission failed\n");
					return 3;
				}
			}
			ece391_tx(0xFE);
			com_flag++;
			turn = !turn;
		} else if(turn == 0) {
			for(i=0;un_flag==0;i++) {
				c = (uint8_t)ece391_rx();
				switch(c) {
					case 0xFE: un_flag = 1;
						break;
					case 0xFF:
						ece391_fdputs (1, (uint8_t*)"Connection timed out\n");
						return 3;
					default: buf2[i] = c;
						ece391_tx((char)0xFA);
						break;
				}
			}
			com_flag++;
			turn = !turn;
		}
	}
	
	while(1) {
		if(turn) {
			ece391_fdputs (1, buf);
			ece391_fdputs (1, (uint8_t*)"> ");
			if (-1 == (cnt = ece391_read (0, buf3, BUFSIZE-1))) {
				ece391_fdputs (1, (uint8_t*)"Can't read from keyboard.\n");
				return 3;
			}
			for(i=0;buf3[i] != 0 &&  buf3[i] != '\n';i++) {
				ece391_tx(buf3[i]);
				c = ece391_rx();
				if(c!=0xFA) {
					ece391_fdputs (1, (uint8_t*)"Transmission failed\n");
					return 3;
				}
			}
			if(0 == ece391_strcmp(buf4,(uint8_t*)"exit")) {
				ece391_tx(0xFD);
				return 0;
			}
			ece391_tx(0xFE);
			turn = !turn;
		} else {
			for(i=0;i<BUFSIZE;i++) {
				buf4[i] = 0;
			}
			ece391_fdputs (1, buf2);
			ece391_fdputs (1, (uint8_t*)"> ");
			un_flag = 0;
			for(i=0;un_flag==0;i++) {
				c = (uint8_t)ece391_rx();
				switch(c) {
					case 0xFE: un_flag = 1;
						break;
					case 0xFD: ece391_fdputs (1, buf4);
						ece391_fdputs (1, (uint8_t*)"\n");
						return 0;
					case 0xFF:
						ece391_fdputs (1, (uint8_t*)"Connection timed out\n");
						return 3;
					default: buf4[i] = c;
						ece391_tx((char)0xFA);
						break;
				}
			}
			ece391_fdputs (1, buf4);
			ece391_fdputs (1, (uint8_t*)"\n");
			turn = !turn;
		}
	}
}

