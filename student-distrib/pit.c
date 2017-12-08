#include "pit.h"

void init_PIT(uint32_t freq) {
	uint32_t divisor;
	divisor = 1193180/freq;
	uint8_t command = 0x34;
	// divisor = 0xFFFF;
	outb(command,0x43);
	uint8_t low_byte = (uint8_t) (divisor &0xFF);
	uint8_t high_byte = (uint8_t) ((divisor>>8) &0xFF);
	outb(low_byte,0x40);
	outb(high_byte,0x40);
}

void pit_irq() {
	int counter = term_pit;
	int old = term_pit;
	counter++;
	counter %= 3;
	int i;
	for(i=0;i<3;i++) {
		if(term[counter].pcb_live == NULL) {
			counter++;
			counter %= 3;
			continue;
		} else {
			context_switch_pit(old,counter);
			break;
		}
	}
	send_eoi(PIT_IRQ);
	return;
}

