#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
    int32_t i,cnt;
    uint8_t buf[1024];
	uint8_t c;
	
	
    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
		return 3;
    }
	
	cnt = 0;
	for(i=0;buf[i] != 0x20 && buf[i] != 0 &&  buf[i] != '\n';i++) {
		if(buf[i] < 0x30 || buf[i] > 0x39) {
			 ece391_fdputs (1, (uint8_t*)"Argument should be a number\n");
			 return 3;
		}
		cnt *= 10;
		cnt += buf[i] - 0x30;
	}

	for(i=0;i<cnt;i++) {
		c = (uint8_t)ece391_rx();
		switch(c) {
			case 0xFE: c = '\n';
				ece391_write(1,&c,1);
				return 0;
			case 0xFC: ece391_tx((char)0xFA);
				break;
			case 0xFF:
				ece391_fdputs (1, (uint8_t*)"Connection timed out\n");
				return 3;
			default: ece391_write(1,&c,1);
				ece391_tx((char)0xFA);
				break;
		}
	}
	return 0;
}

