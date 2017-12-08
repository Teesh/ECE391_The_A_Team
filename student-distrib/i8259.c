/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/*
	i8259_init - initialize PICs
	inputs - none
	outputs - none
	side effects - Master and Slave PICS set and masked
*/
void
i8259_init(void)
{
	unsigned char master_save, slave_save;
	master_save = inb(MASTER_8259_PORT2);
	slave_save = inb(SLAVE_8259_PORT2);
	
	outb(MASK_ALL, MASTER_8259_PORT2);//masks
	outb(MASK_ALL, SLAVE_8259_PORT2);
	
	outb(ICW1,MASTER_8259_PORT);
	outb(ICW2_MASTER,MASTER_8259_PORT2);
	outb(ICW3_MASTER,MASTER_8259_PORT2);
	outb(ICW4,MASTER_8259_PORT2);
	
	outb(ICW1,SLAVE_8259_PORT);
	outb(ICW2_SLAVE,SLAVE_8259_PORT2);
	outb(ICW3_SLAVE,SLAVE_8259_PORT2);
	outb(ICW4,SLAVE_8259_PORT2);
	
	outb(MASK_ALL, MASTER_8259_PORT2); // f9 to mask all but IRQ1,2
	outb(MASK_ALL, SLAVE_8259_PORT2); // fe to mask all but IRQ0
}

/* Enable (unmask) the specified IRQ */
/*
	enable_irq - unmask IRQ #
	inputs - irq_num: the corresponding number for the IRQ
	outputs - none
	side effects - IRQ # is enabled
*/
void
enable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
	// enable specific irq by setting bit to 0
    if(irq_num < MAX_IRQ) {
        port = MASTER_8259_PORT2;
    } else {
        port = SLAVE_8259_PORT2;
        irq_num -= MAX_IRQ;
    }
    value = inb(port) & ~(1 << irq_num);
    outb(value,port); 
}

/* Disable (mask) the specified IRQ */
/*
	disable_irq - mask IRQ #
	inputs - irq_num: the corresponding number for the IRQ
	outputs - none
	side effects - IRQ # is disabled
*/
void
disable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
	// disable specific irq by setting bit to 1
    if(irq_num < MAX_IRQ) {
        port = MASTER_8259_PORT2;
    } else {
        port = SLAVE_8259_PORT2;
        irq_num -= MAX_IRQ;
    }
    value = inb(port) | (1 << irq_num);
    outb(value,port); 
}

/* Send end-of-interrupt signal for the specified IRQ */
/*
	send_eoi - send the EOI
	inputs - irq_num: the corresponding number for the IRQ
	outputs - none
	side effects - EOI is sent to corresping IRQs
*/
void
send_eoi(uint32_t irq_num)
{
	int temp = 0;

	if(irq_num < MAX_IRQ) {//check if on master 
		temp = irq_num | EOI;
		outb(temp,MASTER_8259_PORT);
	} else {
		temp = (irq_num - MAX_IRQ) | EOI;//find which port from slave 
		outb(temp,SLAVE_8259_PORT);
		temp = SLAVE_IRQ | EOI;
		outb(temp,MASTER_8259_PORT);
	}
	
}

