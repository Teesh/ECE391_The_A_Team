#include "idt.h"

/*
	load_idt - initialize IDT
	inputs - none
	outputs - none
	side effects - IDT initialized
*/
void load_idt() {
	int i;
	
	for(i=0;i<NUM_VEC;i++) {
		idt[i].present = 0;
		idt[i].reserved4 = 0;
		idt[i].size = 1;
		idt[i].reserved0 = 1;
		idt[i].dpl = 3;
	}
	// starting at page 165 in Vol 3
	for(i=0;i<20;i++) {
		if(i==15) {
			//EXCEPTION_TABLE++;
			continue;
		}
		SET_IDT_ENTRY(idt[i],EXCEPTION_TABLE+i*8);
		idt[i].present = 1;
		idt[i].dpl = 0;
		idt[i].reserved3 = 0;
		idt[i].reserved2 = 1;
		idt[i].reserved1 = 1;
		idt[i].reserved0 = 0;
		idt[i].seg_selector = KERNEL_CS;	
	}
	
	SET_IDT_ENTRY(idt[RTC_IDT],RTC_HANDLER_WRAP);
	idt[RTC_IDT].present = 1;
	idt[RTC_IDT].dpl = 0;
	idt[RTC_IDT].reserved3 = 0;
	idt[RTC_IDT].reserved2 = 1;
	idt[RTC_IDT].reserved1 = 1;
	idt[RTC_IDT].reserved0 = 0;
	idt[RTC_IDT].seg_selector = KERNEL_CS;
	
	SET_IDT_ENTRY(idt[KEY_IDT], KEY_HANDLER_WRAP);
	idt[KEY_IDT].present = 1;
	idt[KEY_IDT].dpl = 0;
	idt[KEY_IDT].reserved3 = 0;
	idt[KEY_IDT].reserved2 = 1;
	idt[KEY_IDT].reserved1 = 1;
	idt[KEY_IDT].reserved0 = 0;
	idt[KEY_IDT].seg_selector = KERNEL_CS;
	// Exceptions 32 - 255 empty
	
	SET_IDT_ENTRY(idt[MOUSE_IDT], MOUSE_HANDLER_WRAP);
	idt[MOUSE_IDT].present = 1;
	idt[MOUSE_IDT].dpl = 0;
	idt[MOUSE_IDT].reserved3 = 0;
	idt[MOUSE_IDT].reserved2 = 1;
	idt[MOUSE_IDT].reserved1 = 1;
	idt[MOUSE_IDT].reserved0 = 0;
	idt[MOUSE_IDT].seg_selector = KERNEL_CS;
	
	SET_IDT_ENTRY(idt[INT80_IDT], SYSCALL);
	idt[INT80_IDT].present = 1;
	idt[INT80_IDT].dpl = 3;
	idt[INT80_IDT].reserved3 = 1;
	idt[INT80_IDT].reserved2 = 1;
	idt[INT80_IDT].reserved1 = 1;
	idt[INT80_IDT].reserved0 = 0;
	idt[INT80_IDT].seg_selector = KERNEL_CS;
	
	SET_IDT_ENTRY(idt[PIT_IDT], PIT_IRQ_HANDLER);
	idt[PIT_IDT].present = 1;
	idt[PIT_IDT].dpl = 0;
	idt[PIT_IDT].reserved3 = 0;
	idt[PIT_IDT].reserved2 = 1;
	idt[PIT_IDT].reserved1 = 1;
	idt[PIT_IDT].reserved0 = 0;
	idt[PIT_IDT].seg_selector = KERNEL_CS;
	// Exceptions 32 - 255 empty
	lidt(idt_desc_ptr);
}

// interrupt 0 - divide error (DE)
void exception_DE() {
	printf("Divide Error Exception\n");
	halt(1);
}
// interrupt 1 - debug (DB)
void exception_DB() {
	printf("DB Exception\n");
	halt(1);
}
// interrupt 2 - NMI interrupt
void exception_NMI() {
	printf("NMI Exception\n");
	halt(1);
}
// interrupt 3 - Breakpoint (BP)
void exception_BP() {
	printf("BP Exception");
	halt(1);
}
// interrupt 4 - Overflow (OF)
void exception_OF() {
	printf("OF Exception\n");
	halt(1);
}
// interrupt 5 - BOUND Range Exceeded (BR)
void exception_BR() {
	printf("BR Exception\n");
	halt(1);
}
// interrupt 6 - Invalid Opcode (UD)
void exception_UD() {
	printf("UD Exception\n");
	halt(1);
}
// interrupt 7 - Device Not Available (NM)
void exception_NM() {
	printf("NM Exception\n");
	halt(1);
}
// interrupt 8 - Double Fault (DF)
void exception_DF() {
	printf("DF Exception\n");
	halt(1);
}
// interrupt 9 - Coprocessor Segment Overrun
void exception_CSO() {
	printf("CSO Exception\n");
	halt(1);
}
// interrupt 10 - Invalid TSS (TS)
void exception_TS() {
	printf("TS Exception\n");
	halt(1);
}
// interrupt 11 - Segment Not Present (NP)
void exception_NP() {
	printf("NP Exception\n");
	halt(1);
}
// interrupt 12 - Stack Fault (SS)
void exception_SS() {
	printf("SS Exception\n");
	halt(1);
}
// interrupt 13 - General Protection (GP)
void exception_GP() {
	printf("GP Exception\n");
	halt(1);
}
// interrupt 14 - Page-Fault (PF)
void exception_PF() {
	printf("PF Exception\n");
	halt(1);
}
// interrupt 16 - x87 FPU Floating-Point Error (MF)
void exception_MF() {
	printf("MF Exception\n");
	halt(1);
}
// interrupt 17 - Alignment Check (AC)
void exception_AC() {
	printf("AC Exception\n");
	halt(1);
}
// interrupt 18 - Machine-Check (MC)
void exception_MC() {
	printf("MC Exception\n");
	halt(1);
}
// interrupt 19 - SIMD Floating-Point (XF)
void exception_XF() {
	printf("XF Exception\n");
	halt(1);
}
