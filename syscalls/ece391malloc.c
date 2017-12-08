#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
    int32_t i,j,cnt;
	void * ptr;
    uint8_t buf[1024];

    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
		return 3;
    }
	
	for(i=0,cnt=0;buf[i] != 0x20 && buf[i] != 0;i++ &&  buf[i] != '\n') {
		if(buf[i] < 0x30 || buf[i] > 0x39) {
			 ece391_fdputs (1, (uint8_t*)"Argument should be a number\n");
			 return 3;
		}
		cnt *= 10;
		cnt += buf[i] - 0x30;
	}
	
	if((ptr = ece391_malloc(cnt)) == 0) {
		ece391_fdputs (1, (uint8_t*)"Malloc failed\n");
		return 3;
	}
	
	buf[0] = '0';
	buf[1] = 'x';
	cnt = (uint32_t)ptr;
	uint8_t temp ;
	
	for(i=2,j=7;cnt >= 0 && j>=0;i++,j--) {
		temp = (cnt >> (4*j))% 16;				// parse 4 bits at a time, starting from left most, j loops 7->0
		if(temp>9)	buf[i] = 0x61+(temp-10);	// a~f  , 'a' = 0x61 
		else buf[i]=0x30 + temp;				// 0~9	, '0' = 0x30
	}
	buf[10] = '\n';								// end of buf
	
	if (-1 == ece391_write (1, buf, 11)) {
	    return 3;
    }
	
    return 0;
}

