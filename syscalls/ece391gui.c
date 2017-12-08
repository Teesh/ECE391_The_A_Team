#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
	uint32_t i=0, sec=0;
	int ret_val;
    int garbage;
    int rtc_fd;
	rtc_fd = ece391_open((uint8_t*)"rtc");
    ret_val = 1024;
    ret_val = ece391_write(rtc_fd, &ret_val, 4);
	
	while(1) {
		ece391_read(rtc_fd, &garbage, 4);
		// if(i%32 == 0) {
			ece391_clock(sec);
			if(i%154 == 0) sec++;
		// }
		i++;
	}
}

