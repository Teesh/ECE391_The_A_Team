#ifndef _PAGING_H
#define _PAGING_H

#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "pit.h"

#define FOURKILO 	4096
#define ONEKILO		1024
#define FOURMEG		0x400000
#define EXE_IDX		32
#define VID_IDX		33
#define VIDMAP_PAGE 0x84B8000
#define KVID_IDX	(VIDEO>>12)
#define LOW_10		0x3FF
#define LOW_8		0x1FF
#define VID_TERM1	0xD1000
#define VID_TERM2	0xD2000
#define VID_TERM3	0xD3000
// malloc defines
#define MAX_CACHE_IDX 255
#define MAX_CACHE_SIZE 64

typedef struct mem_list {
	void* mem_start;
	void* mem_end;
	struct mem_list* next;
} mem_list_t;

//entry in page table
typedef struct __attribute__((packed)) page
{
	uint32_t present		: 1;
	uint32_t read_write 	: 1;
	uint32_t user_supervisor: 1;
	uint32_t write_through 	: 1;
	uint32_t cache_disable 	: 1;
	uint32_t accessed 		: 1;
	uint32_t dirty 			: 1;
	uint32_t pat 			: 1;
	uint32_t global 		: 1;
	uint32_t available 		: 3;
	uint32_t addr 			: 20;
} page_table_entry_t;

//directory entry for 4KB Page 
typedef struct __attribute__((packed)) page_directory_4kb {
    uint32_t present        : 1;
    uint32_t read_write     : 1;
    uint32_t user_supervisor: 1;
    uint32_t write_through  : 1;
    uint32_t cache_disable  : 1;
    uint32_t accessed       : 1;
    uint32_t reserved       : 1;
    uint32_t page_size      : 1;
    uint32_t global         : 1;
    uint32_t available      : 3;
    uint32_t addr           : 20;
} page_directory_entry_4kb_t;

//directory entry for 4MB Page 
typedef struct __attribute__((packed)) page_directory_4mb
{
    uint32_t present        : 1;
    uint32_t read_write     : 1;
    uint32_t user_supervisor: 1;
    uint32_t write_through  : 1;
    uint32_t cache_disable  : 1;
    uint32_t accessed       : 1;
    uint32_t dirty          : 1;
    uint32_t page_size      : 1;
    uint32_t global         : 1;
    uint32_t available      : 3;
    uint32_t pat            : 1;
    uint32_t reserved       : 9;
    uint32_t addr           : 10;
} page_directory_entry_4mb_t;

//need to combine the kinds of pages entry

void paging_init(void);
void set_page(uint32_t pid);
void flush_tlb(void);
uint32_t map_vid(void);
uint32_t context_switch(uint32_t old_term, uint32_t new_term);
uint32_t context_switch_pit(uint32_t old_term, uint32_t new_term);

void* malloc(uint32_t n_bytes);
void free(uint32_t ptr);
void pmap(void);

extern mem_list_t* mem_head;
extern mem_list_t* mem_tail;

#endif /* _PAGING_H */
