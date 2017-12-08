#define ASM     1

#include "x86_desc.h"
.text


.globl SYSCALL, halt_return, PIT_IRQ_HANDLER

PIT_IRQ_HANDLER:
	pushal
	call pit_irq
	popal
	iret
	
SYSCALL:
	#ERROR CHECK
	#save all regs
	#Check if %eax is valid?
	
	cmpl $0, %eax
	jle done_error
	cmpl $17, %eax
	jg done_error

	#push arg
	pushl %edx
	pushl %ecx	
	pushl %ebx
	decl  %eax
	

	call *SYSCALL_TBL(,%eax,4)
	#pop arg
	addl $12, %esp
	jmp done
	
done_error:
	movl $-1, %eax

done:
	iret

halt_return:
	leave
	ret
	
SYSCALL_TBL:
.long halt,execute,read,write,open,close,getargs,vidmap,set_handler,sigreturn,malloc,pmap,free,clock,parse_pacman,read_serial,write_serial
























