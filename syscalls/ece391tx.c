#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
	uint32_t i;
	uint8_t buf[1024];
	uint8_t c;
	int ret_val;
	int garbage;
    int rtc_fd;
	
    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
		return 3;
    }
	
	rtc_fd = ece391_open((uint8_t*)"rtc");
	
	ece391_tx(0xFC);
	for(i=0;i<100;i++) {
		ece391_read(rtc_fd, &garbage, 4);
	}	
	c = ece391_rx();
	if(c != 0xFA) {
		ece391_fdputs (1, (uint8_t*)"Bad connection\n");
		return 3;
	}
	
	for(i=0;buf[i] != 0 &&  buf[i] != '\n';i++) {
		ece391_tx(buf[i]);
		c = ece391_rx();
		if(c!=0xFA) {
			ece391_fdputs (1, (uint8_t*)"Transmission failed\n");
			return 3;
		}
	}
	ece391_tx(0xFE);
	return 0;
}


