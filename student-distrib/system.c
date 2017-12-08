#include "system.h"

pcb_t* pcb_live = NULL;
static uint32_t pid_max = 0;
static uint8_t arg_buf[RAND];
ops_tbl_t stdin_ops;
ops_tbl_t stdout_ops;
ops_tbl_t rtc_ops;
ops_tbl_t dir_ops;
ops_tbl_t file_ops;
term_t term[3];

void system_init(void) {
	int i;
	for(i = 0;i<3;i++) {
		term[i].active_flag = 0;
		term[i].vid_mem = (void*)(0xD1000 + i*FOURKILO);
		term[i].screen_x = 0;
		term[i].screen_y = 0;
		term[i].pcb_live = NULL;
		term[i].saved_esp = 0;
		term[i].saved_ebp = 0;
	}
}
/*
*input: status - something from the exe
*output: status in eax
*desciption: tear down all data for current process, go back to parent process, check for edge cases, reset stack
*/
int32_t halt (uint8_t status) {
	int i;
	if(term[term_pit].pcb_live->shell_flag == 1) {
		cli();
		clear_buf();
		for(i=LOWFD;i<HIGHFD;i++) {
			if(term[term_pit].pcb_live->file_desc[i].flags == 1) close(i);
		}
		term[term_pit].active_flag = 0;
		term[term_pit].screen_x = 0;
		term[term_pit].screen_y = 0;
		term[term_pit].pcb_live = NULL;
		pid_max--;
		
		
		memset((uint8_t*)0xB8000,0x0,4096);
		
		context_switch(term_pit,0);
		
		term[term_pit].saved_ebp = NULL;
		term[term_pit].saved_esp = NULL;

		
		//exit from terminal to GUI
		if(	isModex == 0){
			set_modex();
			draw_background();
			// mouse_write(0xF4);
			// mouse_read();  
		}
		
		send_eoi(KEY_IRQ);
		sti();
		while(1);
	}
	
	if(term[term_pit].pcb_live->parent == NULL) {
		tss.esp0 = term[term_pit].pcb_live->k_stack_ptr-EIGHT_KB;
	} else {
		for(i=LOWFD;i<HIGHFD;i++) {
			if(term[term_pit].pcb_live->file_desc[i].flags == 1) close(i);
		}
		term[term_pit].pcb_live = term[term_pit].pcb_live->parent;
		set_page(term[term_pit].pcb_live->pid-1);
		tss.esp0 = term[term_pit].pcb_live->k_stack_ptr; // make this point to base of kernel stack
	}
	tss.ss0 = KERNEL_DS;
	pid_max--;
	send_eoi(KEY_IRQ);
	asm volatile(	"movl %0,%%esp \n\t"
					"movl %1,%%ebp \n\t"
					"movb %2,%%al \n\t"
					"jmp halt_return"
					:
					:"r"(term[term_pit].pcb_live->save_stack_ptr),"r"(term[term_pit].pcb_live->save_base_ptr),"r"(status)
				);
	return 0;
}

/*
*input: command - command to execute, if that wasn't obvious enough
*output: it never returns technically
*desciption: sets up pages, stacks, PCBs, and just about everything else for an executable, then jumps into it
*/
int32_t execute (const uint8_t* command) {
	int i;
	uint8_t cmd[MAXBUF];
	
	for(i=0;command[i]!=SPACE_CHAR && command[i]!=NULL;i++) {
		cmd[i] = command[i];
	}
	cmd[i] = NULL;
	if(command[i] != NULL) {
		strcpy((int8_t*)arg_buf,(int8_t*)&command[i+1]);
	} else {
		arg_buf[0] = NULL;
		arg_buf[1] = NULL;
	}
	
	if (pid_max >=PID_MAX)
		return ERR;

	if(file_open(cmd) == -1) return -1;
	unsigned char buffer[EXEC_DATA];
	file_read(-1,buffer,EXEC_DATA);
	// 30 bytes only
	// magic numbers
	// 0x7f, 0x45, 0x4c, 0x46
	if((buffer[EXE_1] != EXE_B1) || (buffer[EXE_2] != EXE_B2) || (buffer[EXE_3] != EXE_B3) || (buffer[EXE_4] != EXE_B4)) 
		return -1;
	
	// 27 to 24 holds start address of exe
	uint32_t entry_addr = 0;
	entry_addr |= buffer[EXE_A1];
	entry_addr |= buffer[EXE_A2] << SECOND_BYTE;
	entry_addr |= buffer[EXE_A3] << THIRD_BYTE;
	entry_addr |= buffer[EXE_A4] << FOURTH_BYTE;
	
	// set up 4MB page in PD, map 128MB (0x08000000) to 8MB

	set_page(pid_max);
	// first shell loaded to 8MB, actual exe at entry_addr, load exe at 0x08048000
	file_read(-1,(unsigned char*)SHELL_LOAD,SHELL_DATA); // maybe?
	
	// PCB stuff here
	// PCB is placed at top of kernel stack and grows downward
	// each process needs a kernel stack of size 8kB starting at the bottom of the 4MB kernel page
	pcb_t pcb;
	pcb.pid = ++pid_max;
	pcb.parent = term[term_live].pcb_live;
	if((term_live == 1 || term_live == 2) && pcb.parent == NULL) {
		pcb.parent = term[0].pcb_live;
		pcb.shell_flag = 1;
	}
	pcb.entry_point = entry_addr;

	if(term[term_live].pcb_live != NULL)
		asm volatile(	"movl %%esp,%0 	\n\t"
						"movl %%ebp,%1	\n\t"
						:"=r"(term[term_live].pcb_live->save_stack_ptr),"=r"(term[term_live].pcb_live->save_base_ptr)
					);					
	pcb.k_stack_ptr = K_STACK_PTR-(EIGHT_KB*(pcb.pid))-4;
	
	// set pcb at top of kernel stack
	pcb_t* pcb_child = memcpy((void*)((pcb.k_stack_ptr+4)-EIGHT_KB),&pcb,sizeof(pcb_t));
	if(term[term_live].pcb_live != NULL) term[term_live].pcb_live->child = pcb_child;
	term[term_live].pcb_live = pcb_child;
	
	term[term_live].pcb_live->curr_fd = 2;
	
	term[term_live].pcb_live->file_desc[STDIN].fops = &stdin_ops;
	term[term_live].pcb_live->file_desc[STDIN].flags = 1;
	term[term_live].pcb_live->file_desc[STDOUT].fops = &stdout_ops;
	term[term_live].pcb_live->file_desc[STDOUT].flags = 1;
	for(i = LOWFD;i < HIGHFD;i++) {
		term[term_live].pcb_live->file_desc[i].flags = 0;
	}
	tss.esp0 = pcb.k_stack_ptr; // make this point to base of kernel stack
	tss.ss0 = KERNEL_DS;
	
	

	
	// ASM to setup EIP,CS,EFLAGS,ESP,SS then call IRET.  Priveledge in low 2 bytes of CS reg
	// inline call to wrapper
	// page 341 Vol3
	asm volatile(	"pushl $0x002B		\n\t"
					"pushl %1			\n\t"
					"pushfl				\n\t"
					"pop %%ebx			\n\t"
					"orl $0x200,%%ebx	\n\t"
					"pushl %%ebx		\n\t"
					"pushl $0x0023		\n\t"
					"pushl %0			\n\t"
					"iret				\n\t"
					:
					: "r"(pcb.entry_point),"r"(STACK_PTR)
					: "ebx"
				);
	return 0;
}

/*
*input: filename - the name of the file
*output: success or failure
*desciption: create a file descriptor for the filename
*/
int32_t open (const uint8_t* filename) {
	int32_t fd;
	for(fd=LOWFD;fd<HIGHFD;fd++) {
		if(term[term_pit].pcb_live->file_desc[fd].flags == 0) break;
	}
	if(fd == HIGHFD) return -1;
	if (compare(filename, (uint8_t*)".") != -1){
		term[term_pit].pcb_live->file_desc[fd].fops = &dir_ops;
		term[term_pit].pcb_live->file_desc[fd].flags = 1;
	} else if (compare(filename, (uint8_t*)"rtc") != -1){
		term[term_pit].pcb_live->file_desc[fd].fops = &rtc_ops;
		term[term_pit].pcb_live->file_desc[fd].flags = 1;
	} else {
		if(file_open(filename) == -1) return -1;
		update_inode(fd);
		term[term_pit].pcb_live->file_desc[fd].fops = &file_ops;
		term[term_pit].pcb_live->file_desc[fd].flags = 1;
		term[term_pit].pcb_live->file_desc[fd].file_pos = 0;
	}
	return fd;
}

/*
*input: fd - file descriptor, buf - buf to fill in case of write, nbytes - not used, YET
*output: number of bytes read
*desciption: read something
*/
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
	if(buf==NULL) return -1;
	if(fd < STDIN || fd > HIGHFD-1) return -1;
	if(term[term_pit].pcb_live->file_desc[fd].flags==0) return -1;
	return term[term_pit].pcb_live->file_desc[fd].fops->read(fd, buf, nbytes);
}

/*
*input: fd - file descriptor, buf - buf to  write, nbytes - nuber of bytes to write
*output: 0 for success, -1 for failure
*desciption: write stuff
*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
	if(buf==NULL) return -1;

	if(fd < STDIN || fd > HIGHFD-1) return -1;
	if(term[term_pit].pcb_live->file_desc[fd].flags==0) return -1;

	return term[term_pit].pcb_live->file_desc[fd].fops->write(fd, buf, nbytes);
}

/*
*input: file descriptor
*output: success or failure
*desciption: clean out the file descriptor
*/
int32_t close (int32_t fd) {

	int val = 0;
	if(fd < LOWFD || fd > HIGHFD-1) return -1; 
	if(term[term_pit].pcb_live->file_desc[fd].flags==0) return -1;

	if((val = term[term_pit].pcb_live->file_desc[fd].fops->close(fd)) == -1) return -1;
	
	term[term_pit].pcb_live->file_desc[fd].flags = 0;
	term[term_pit].pcb_live->curr_fd--;
	return val;
}

/*
*input: buf- buffer to fill,nbytes- max buf size
*output: 0 on success -1 on failure 
*desciption: set buffer to arguments for user space
*/
int32_t getargs (uint8_t* buf, int32_t nbytes) {
	if(buf==NULL) return -1;

	int i=0;
	do {
		buf[i] = arg_buf[i];
		i++;
	} while (arg_buf[i]!=NULL && i<nbytes);
	buf[i] = NULL;
	return 0;
}

/*
*input: screen_start
*output: 0 if success -1 for fail
*desciption: set screen_start to the video memory
*/
int32_t vidmap (uint8_t** screen_start) {
	if(screen_start==NULL) return -1;
	if((uint32_t)screen_start<FOURMEG*2) return -1; //if user doesn't have access
	*screen_start = (uint8_t*)map_vid();
	return 0;
}

/*
*input: dunno yet
*output: we'll get to this later
*desciption: RTDC
*/
int32_t set_handler (int32_t signum, void* handler_address) {
	return -1;
}

/*
*input: dunno yet
*output: we'll get to this later
*desciption: RTDC
*/
int32_t sigreturn (void) {
	return -1;
}


