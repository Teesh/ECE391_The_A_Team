#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "lib.h"
#include "paging.h"
#include "file.h"
#include "x86_desc.h"
#include "rtc.h"
#include "terminal.h"
#include "i8259.h"
#include "pit.h"
#include "modex.h"

#define EIGHT_KB	(8*1024)
#define LOWFD		2
#define HIGHFD		8
#define STDIN		0
#define STDOUT		1
#define MAXBUF		32
#define ERR			256
#define RAND		200
#define SPACE_CHAR	0x20
#define PID_MAX		6
#define SHELL_DATA	(40*1024)					// size of shell bin
#define SHELL_LOAD	0x08048000					// point to load the exe bin
#define K_STACK_PTR 0x00800000					// bottom of Kernel page
#define STACK_PTR	(0x8000000 + 0x400000 -4)	// bottom of page (-4B to align)
#define EXEC_DATA	40							// size of exe metadata
#define EXE_1		0
#define EXE_2		1
#define EXE_3		2
#define EXE_4		3
#define EXE_B1		0x7f						// exe magic numbers
#define EXE_B2		0x45
#define EXE_B3		0x4c
#define EXE_B4		0x46
#define EXE_A1		24							// indexes of entry point pointer
#define EXE_A2		25
#define EXE_A3		26
#define EXE_A4		27
#define SECOND_BYTE	8
#define THIRD_BYTE	16
#define FOURTH_BYTE	24

typedef struct ops_tbl {
	int32_t (*open)(const uint8_t*); // set to function pointer syntax for all
	int32_t (*read) (int32_t, void* , int32_t );
	int32_t (*write)(int32_t, const void*, int32_t);
	int32_t (*close)(int32_t);
} ops_tbl_t;

typedef struct file_desc {
	ops_tbl_t* fops;
	uint32_t inode;
	uint32_t file_pos;
	uint32_t flags;
} file_desc_t;

// PCB (process control block)
typedef struct pcb {
	uint32_t pid;			
	struct pcb* parent;	
	struct pcb* child;
	uint32_t entry_point;
	uint32_t save_stack_ptr;
	uint32_t save_base_ptr;
	uint32_t k_stack_ptr;
	uint32_t shell_flag;
	uint32_t curr_fd;
	file_desc_t file_desc[HIGHFD];
} pcb_t;

typedef struct term {
	uint32_t active_flag;
	pcb_t* pcb_live;
	void* vid_mem;
	uint32_t screen_x;
	uint32_t screen_y;
	uint32_t saved_esp;
	uint32_t saved_ebp;
} term_t;

extern void LOAD_USR();
extern void SYSCALL();
extern ops_tbl_t stdin_ops;
extern ops_tbl_t stdout_ops;
extern ops_tbl_t rtc_ops;
extern ops_tbl_t dir_ops;
extern ops_tbl_t file_ops;
extern pcb_t* pcb_live;
extern term_t term[3];

int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);
void system_init(void);

#endif /* _SYSTEM_H */
