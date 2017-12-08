#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
	uint32_t i,cnt;
	uint8_t buf[1024],buf2[40];
	
    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
		return 3;
    }
	
	if(buf[0] != '0' || buf[1] != 'x') {
		ece391_fdputs (1, (uint8_t*)"Argument should be in hex\n");
		return 3;
	}
	
	for(i=2,cnt=0;buf[i] != 0x20 && buf[i] != 0 && buf[i] != '\n';i++ &&  buf[i] != '\n') {
		if(buf[i] >= 0x30 && buf[i] <= 0x39){
			cnt *= 16;
			cnt += buf[i]-0x30;
		} else if(buf[i] >= 0x41 && buf[i] <= 0x46) {
			cnt *= 16;
			cnt += buf[i]-0x37;
		} else {
			ece391_fdputs (1, (uint8_t*)"Argument should be a hex number\n");
			return 3;
		}
	}
	// buf[i] = '\0';
	// ece391_itoa(cnt,buf2,16);
	// buf[14] = '\n';
	// ece391_write(1,buf,15);
	ece391_free(cnt);
	
	
    return 0;
}

