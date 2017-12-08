
.text

.globl EXCEPTION_TABLE, RTC_HANDLER_WRAP, KEY_HANDLER_WRAP, MOUSE_HANDLER_WRAP

MOUSE_HANDLER_WRAP:
	pushal
	call mouse_handler
	popal
	iret

 # wrapper for RTC handler
RTC_HANDLER_WRAP:
	pushal
	call rtc_handler
	popal
	iret
	
 # wrapper for Keyboard handler
KEY_HANDLER_WRAP:
	pushal
	call key_handler
	popal
	iret


 # table of all exceptions needed to be populated in IDT
EXCEPTION_TABLE:
.long	EXCEPTION_DE_WRAP, EXCEPTION_DB_WRAP, EXCEPTION_NMI_WRAP, EXCEPTION_BP_WRAP, EXCEPTION_OF_WRAP,	EXCEPTION_BR_WRAP, EXCEPTION_UD_WRAP, EXCEPTION_NM_WRAP, EXCEPTION_DF_WRAP, EXCEPTION_CSO_WRAP, EXCEPTION_TS_WRAP, EXCEPTION_NP_WRAP, EXCEPTION_SS_WRAP, EXCEPTION_GP_WRAP, EXCEPTION_PF_WRAP, blank, EXCEPTION_MF_WRAP, EXCEPTION_AC_WRAP, EXCEPTION_MC_WRAP, EXCEPTION_XF_WRAP

blank:
	iret
 # exception 0	
EXCEPTION_DE_WRAP:
	pushal
	call exception_DE
	popal
	iret
 # exception 1	
EXCEPTION_DB_WRAP:
	pushal
	call exception_DB
	popal
	iret
 # exception 2	
EXCEPTION_NMI_WRAP:
	pushal
	call exception_NMI
	popal
	iret
 # exception 3	
EXCEPTION_BP_WRAP:
	pushal
	call exception_BP
	popal
	iret
 # exception 4	
EXCEPTION_OF_WRAP:
	pushal
	call exception_OF
	popal
	iret
 # exception 5	
EXCEPTION_BR_WRAP:
	pushal
	call exception_BR
	popal
	iret
 # exception 6	
EXCEPTION_UD_WRAP:
	pushal
	call exception_UD
	popal
	iret
 # exception 7	
EXCEPTION_NM_WRAP:
	pushal
	call exception_NM
	popal
	iret
 # exception 8	
EXCEPTION_DF_WRAP:
	pushal
	call exception_DF
	popal
	iret
 # exception 9	
EXCEPTION_CSO_WRAP:
	pushal
	call exception_CSO
	popal
	iret
 # exception 10	
EXCEPTION_TS_WRAP:
	pushal
	call exception_TS
	popal
	iret
 # exception 11	
EXCEPTION_NP_WRAP:
	pushal
	call exception_NP
	popal
	iret
 # exception 12	
EXCEPTION_SS_WRAP:
	pushal
	call exception_SS
	popal
	iret
 # exception 13
EXCEPTION_GP_WRAP:
	pushal
	call exception_GP
	popal
	iret
 # exception 14	
EXCEPTION_PF_WRAP:
	pushal
	call exception_PF
	popal
	iret
 # exception 16	
EXCEPTION_MF_WRAP:
	pushal
	call exception_MF
	popal
	iret
 # exception 17	
EXCEPTION_AC_WRAP:
	pushal
	call exception_AC
	popal
	iret
 # exception 18	
EXCEPTION_MC_WRAP:
	pushal
	call exception_MC
	popal
	iret
 # exception 19	
EXCEPTION_XF_WRAP:
	pushal
	call exception_XF
	popal
	iret
