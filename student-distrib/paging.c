#include "paging.h"
#define VIDEO 0xB8000

uint32_t mem_cache = 60;
mem_list_t* mem_head = NULL;
mem_list_t* mem_tail = NULL;
mem_list_t* list_addr = (mem_list_t*)(34*FOURMEG);
uint32_t list_idx = 0;

static page_directory_entry_4mb_t page_directory[ONEKILO] __attribute__((aligned(FOURKILO)));
static page_table_entry_t page_table_4kb[ONEKILO] __attribute__((aligned(FOURKILO)));
static page_table_entry_t vidmap_table[ONEKILO] __attribute__((aligned(FOURKILO)));

uint32_t temp; // used for doing math on addresses

static unsigned char term2_buf[BUILD_BUF_SIZE] ;



char * temp_vid_ptr = (char*) VIDEO;

uint32_t * test;
int i_test;



/*
	paging_init - initialize and enable paging
	inputs - none
	outputs - none
	side effects - paging enabled
*/
void paging_init(){
    int i;
    
    for (i = 0 ; i < ONEKILO; i ++){
        // Initialize page directory:
        page_directory[i].present = 0;          // Set to not present
        page_directory[i].read_write = 1;       // Enable R/W
        page_directory[i].user_supervisor = 0;  // Only kernel-mode can access them
        page_directory[i].write_through = 0;
        page_directory[i].cache_disable = 0;
        page_directory[i].accessed = 0;
        page_directory[i].dirty = 0;
        page_directory[i].page_size = 1;        // Each entry is pointing to a 4 MB page
        page_directory[i].global = 0;
        page_directory[i].available = 0;
        page_directory[i].pat = 0;
        page_directory[i].reserved = 0;
        page_directory[i].addr = ((i * FOURMEG) >> 22) & LOW_10;  //each page has size 4MB
                                                                // Shift by 22 to get 10 MSB of address of page_table_4kb
                                                                // LOW_10: mask to get only 10 LSB

        // Initialize page table
        page_table_4kb[i].present = 0;              // Set to not present
        page_table_4kb[i].read_write = 0;           // Enable R/W
        page_table_4kb[i].user_supervisor = 0;      // Only kernel-mode can access them
        page_table_4kb[i].write_through = 0;
        page_table_4kb[i].cache_disable = 0;
        page_table_4kb[i].accessed = 0;
        page_table_4kb[i].dirty = 0;
        page_table_4kb[i].pat = 0;
        page_table_4kb[i].global = 0;
        page_table_4kb[i].available = 0;
        page_table_4kb[i].addr = ((i * FOURKILO) >> 12) & 0x0FFFFF; // each page has size 4kB
                                                                    // Shift by 12 to get 20 MSB of address of page_table_4kb
                                                                    // 0x0FFFFF: mask to get only 20 LSB
		
		vidmap_table[i].present = 0;              // Set to not present
        vidmap_table[i].read_write = 0;           // Enable R/W
        vidmap_table[i].user_supervisor = 1;      // Only kernel-mode can access them
        vidmap_table[i].write_through = 0;
        vidmap_table[i].cache_disable = 0;
        vidmap_table[i].accessed = 0;
        vidmap_table[i].dirty = 0;
        vidmap_table[i].pat = 0;
        vidmap_table[i].global = 0;
        vidmap_table[i].available = 0;
        vidmap_table[i].addr = ((i * FOURKILO) >> 12) & 0x0FFFFF; // each page has size 4kB
                                                                    // Shift by 12 to get 20 MSB of address of vidmap_table
                                                                    // 0x0FFFFF: mask to get only 20 LSB

		
    }
	
	for(i=16; i<=48; i++ )
	{
		page_table_4kb[i].present = 1;              // Set to not present
        page_table_4kb[i].read_write = 1;           // Enable R/W
        page_table_4kb[i].user_supervisor = 1;      // Only kernel-mode can access them
        page_table_4kb[i].write_through = 0;
        page_table_4kb[i].cache_disable = 0;
        page_table_4kb[i].accessed = 0;
        page_table_4kb[i].dirty = 0;
        page_table_4kb[i].pat = 0;
        page_table_4kb[i].global = 0;
        page_table_4kb[i].available = 0;
        page_table_4kb[i].addr = ((i * FOURKILO) >> 12) & 0x0FFFFF;
	}

	
	for(i=160; i<=192; i++ )
	{
		page_table_4kb[i].present = 1;              // Set to not present
        page_table_4kb[i].read_write = 1;           // Enable R/W
        page_table_4kb[i].user_supervisor = 1;      // Only kernel-mode can access them
        page_table_4kb[i].write_through = 0;
        page_table_4kb[i].cache_disable = 0;
        page_table_4kb[i].accessed = 0;
        page_table_4kb[i].dirty = 0;
        page_table_4kb[i].pat = 0;
        page_table_4kb[i].global = 0;
        page_table_4kb[i].available = 0;
        page_table_4kb[i].addr = ((i * FOURKILO) >> 12) & 0x0FFFFF;
	}

   // page_directory[0]: page of 0-4MB  : contain Video Memory
   // page_directory[1]: page of 4-8MB  : contain kernel
    
    page_directory[0].present = 1;
    page_directory[0].page_size = 0;          //Clear PS flag to set page size is 4 KBytes and the page-directory entry points to a page table
    temp = (int)&page_table_4kb;
	
    //Set page_directory[0], such that 20 MSB is the address points to page_table_4kb
    page_directory[0].addr = (temp >> 22) & LOW_10 ; // Shift by 22 to get bit [31:22] of address of page_table_4kb
                                                              // LOW_10: mask to get only 10 LSB after shifting
   
    page_directory[0].reserved = (temp >> 13) & LOW_8; // Shift by 13 to get bit [21:13] of address of page_table_4kb
                                                                 // LOW_8: mask to get only 9 LSB after shifting

    page_directory[0].pat  =  (temp >> 12) & 0x01;    // Shift by 12 to get bit 12 of address of page_table_4kb
                                                                // 0x01: mask to get only LSB after shifting
    
    
    // Set the 4kB page of video memory to be present
    page_table_4kb[KVID_IDX].present = 1;          //VIDEO is virtual address of video memory defined in lib.c.
    page_table_4kb[KVID_IDX].read_write = 1;       // Shift by 12 to get page#
    
    page_directory[1].present = 1;
	
	// set up page table for vidmap here
	temp = (int)&vidmap_table;
	page_directory[VID_IDX].present = 1;
	page_directory[VID_IDX].page_size = 0;
	page_directory[VID_IDX].user_supervisor = 1;
	
	page_directory[VID_IDX].addr = (temp >> 22) & LOW_10 ;	// Shift by 22 to get bit [31:22] of address of page_table_4kb
                                                            // LOW_10: mask to get only 10 LSB after shifting
    page_directory[VID_IDX].reserved = (temp >> 13) & LOW_8; // Shift by 13 to get bit [21:13] of address of page_table_4kb
                                                            // LOW_8: mask to get only 9 LSB after shifting
    page_directory[VID_IDX].pat  =  (temp >> 12) & 0x01;    // Shift by 12 to get bit 12 of address of page_table_4kb
                                                            // 0x01: mask to get only LSB after shifting
	vidmap_table[KVID_IDX].present = 1;          //VIDEO is virtual address of video memory defined in lib.c.
    vidmap_table[KVID_IDX].read_write = 1;
	
	
	
	
	
	for(i = 0xD1; i <=0xD3; i ++){
		page_table_4kb[i].present = 1;              // Set to not present
        page_table_4kb[i].read_write = 1;           // Enable R/W
        page_table_4kb[i].user_supervisor = 1;      // Only kernel-mode can access them
        page_table_4kb[i].write_through = 0;
        page_table_4kb[i].cache_disable = 0;
        page_table_4kb[i].accessed = 0;
        page_table_4kb[i].dirty = 0;
        page_table_4kb[i].pat = 0;
        page_table_4kb[i].global = 0;
        page_table_4kb[i].available = 0;
        page_table_4kb[i].addr = ((i * FOURKILO) >> 12) & 0x0FFFFF;
		
	}
	
	
	// page_table_4kb[0xD1].present = 1;              // Set to not present
	 // page_table_4kb[0xD1].read_write = 1;           // Enable R/W
    // page_table_4kb[0xD1].user_supervisor = 1;      // Only kernel-mode can access them
	
	// page_table_4kb[0xC1].user_supervisor = 1;
	// page_table_4kb[0xC1].present = 1;          //VIDEO is virtual address of video memory defined in lib.c.
    // page_table_4kb[0xC1].read_write = 1;
	// page_table_4kb[0xC2].user_supervisor = 1;
	// page_table_4kb[0xC2].present = 1;          //VIDEO is virtual address of video memory defined in lib.c.
    // page_table_4kb[0xC2].read_write = 1;
	// page_table_4kb[0xC3].user_supervisor = 1;
	// page_table_4kb[0xC3].present = 1;          //VIDEO is virtual address of video memory defined in lib.c.
    // page_table_4kb[0xC3].read_write = 1;
	
	page_table_4kb[0xD1].addr = 0xB8;
	page_table_4kb[0xB8].addr = 0xB8;
	
	
	page_directory[34].present=1; //page for modex
	page_directory[34].read_write=1;
	page_directory[34].user_supervisor=1;
	page_directory[34].page_size=1;
	
	// inline ASM pulled straight from OSDev
	asm volatile("movl	%0,%%eax 				\n\t"
				"movl	%%eax,%%cr3				\n\t"
				"movl	%%cr4,%%eax				\n\t"
				"orl	$0x00000010,%%eax	\n\t"
				"movl	%%eax,%%cr4             \n\t"
				"movl	%%cr0,%%eax				\n\t"
				"orl	$0x80000000,%%eax		\n\t"
				"movl	%%eax,%%cr0				\n\t"
				
				: 
				: "r"(page_directory)
				:"eax","cc","memory"
	);
				
}
// flush tlb on any changes to paging after init


/*
*input: pid - the process ID, but functionally always pid-1
*output: nothing
*desciption: change the page to point to the correct physical memory (8MB,12MB,etc.), and flush TLBs
*/
void set_page(uint32_t pid) {
	page_directory[EXE_IDX].present = 1;
	page_directory[EXE_IDX].addr = (((pid+2) * FOURMEG) >> 22) & LOW_10;
	page_directory[EXE_IDX].user_supervisor = 1;
	flush_tlb();
}
/*
*input:none
*output: address of Video Page for User
*desciption: create a video page for user and flush TLBs
*/
uint32_t map_vid() {
	vidmap_table[KVID_IDX].addr = KVID_IDX;
	flush_tlb();
	return VIDMAP_PAGE;
}
/*
*input: none
*output: nothing
*desciption: Flush TLBS
*/
void flush_tlb() {
	asm volatile("movl	%0,%%eax 				\n\t"
				"movl	%%eax,%%cr3				\n\t"
				: 
				: "r"(page_directory)
				:"eax","cc","memory"
	);
}


uint32_t context_switch(uint32_t old_term, uint32_t new_term) {
	// cli();
	if(old_term == new_term) return 0;
	term_live = new_term;
	
	page_table_4kb[(old_term + 0xD1)].addr = (old_term + 0xD1);
	flush_tlb();
	
	memcpy(term[old_term].vid_mem,(void*)VIDEO,FOURKILO);
	memcpy((void*)VIDEO,term[new_term].vid_mem,FOURKILO);
	page_table_4kb[(new_term + 0xD1)].addr = (VIDEO >> 12);
	flush_tlb();

	cursor(term[new_term].screen_x,term[new_term].screen_y);

	if(( new_term == 1 || new_term == 2) && (term[1].active_flag == 0  && term[2].active_flag == 0 )){
			term[0].saved_ebp = NULL;
			term[0].saved_esp = NULL;
	}
	
	
	if(term[new_term].pcb_live == NULL){
		clear();
		cursor(0,0);
		term[new_term].active_flag = 1;
		
		// if(term[old_term].saved_esp == 0 || term[old_term].saved_ebp == 0 || term[old_term].active_flag == 0) {
			asm volatile(	"pushal			\n\t"
							"movl %%esp,%0 	\n\t"
							"movl %%ebp,%1	\n\t"
							:"=r"(term[term_pit].saved_esp),"=r"(term[term_pit].saved_ebp)
						);
		// }
		
		term_pit = new_term;
		send_eoi(KEY_IRQ);
		if(new_term == 1) {
			execute((uint8_t*)"shell");
		} else if(new_term == 2) {
			execute((uint8_t*)"pacman");
		}
	}
	else if(old_term == 2 && new_term ==0){
		memcpy((void*) term2_buf,(void*)build_ptr, BUILD_BUF_SIZE);
	}
	else if(old_term == 0&& new_term == 2){
		memcpy((void*)build_ptr, (void*) term2_buf, BUILD_BUF_SIZE);
		show_screen();
	}

	// sti();
	return 0;
}


uint32_t context_switch_pit(uint32_t old_term, uint32_t new_term) {
	//Stop writing to screen after switch from 1-->0
	if(old_term == new_term) return 0;
	cli();
	
	term_pit = new_term;
	asm volatile(	"pushal			\n\t"
					"movl %%esp,%0 	\n\t"
					"movl %%ebp,%1	\n\t"
					:"=r"(term[old_term].saved_esp),"=r"(term[old_term].saved_ebp)
				);
	
	set_page(term[term_pit].pcb_live->pid-1);
	tss.esp0 = term[term_pit].pcb_live->k_stack_ptr;
	tss.ss0 = KERNEL_DS;
	

	
	vidmap_table[KVID_IDX].addr = (new_term + 0xD1);
	if(term_live == term_pit) 
		vidmap_table[KVID_IDX].addr = KVID_IDX;

	flush_tlb();

	send_eoi(PIT_IRQ);
	sti();
	asm volatile(	"movl %0,%%esp 	\n\t"
					"movl %1,%%ebp	\n\t"
					"popal			\n\t"
					::"r"(term[new_term].saved_esp),"r"(term[new_term].saved_ebp)
				);
	return 0;
}

void* malloc(uint32_t n_bytes) {
	int i;
	uint32_t start_addr_idx, end_addr_idx; // points to first open addr in mem_cache
	mem_list_t* front, *back, *head = mem_head; // points to head of working terminal linked list
	mem_list_t* new_node = list_addr + list_idx;
	uint32_t free_size;
	void* mem_end;
	void* bottom = (void*)(mem_cache*FOURMEG-1);
	void* top = (void*)((mem_cache-20)*FOURMEG-1);
	if(n_bytes == 0) return NULL;
	
	list_idx++;
	new_node->mem_start = NULL;
	new_node->mem_end = NULL;
	new_node->next = NULL;
	
	if(head == NULL) { // if 0 blocks
		mem_end = bottom;
		mem_tail = new_node;
		mem_head = new_node;
	} else if(head->mem_end != bottom) {
		free_size = (uint32_t)(bottom - head->mem_end);
		if(free_size >= n_bytes) {
			mem_end = bottom;
			new_node->next = head;
			mem_head = new_node;
		}
	} else if(head->next == NULL) { // if one block
		free_size = (uint32_t)(bottom - head->mem_end);
		if(free_size < n_bytes) { // if space not free before first block
			mem_end = head->mem_start + 1;
			head->next = new_node;
			mem_tail = new_node;
		} else { // if space free before first block
			mem_end = bottom;
			new_node->next = head;
			mem_head = new_node;
		}
	} else { // if two blocks
		front = head;
		back = head->next;
		while(back != NULL) { // chekc space between every block
			free_size = (uint32_t)(front->mem_start+1 - back->mem_end);
			if(free_size >= n_bytes) {
				mem_end = front->mem_start;
				new_node->next = back;
				front->next = new_node;
				break;
			}
			front = front->next;
			back = back->next;
		}
		mem_end = front->mem_start + 1;
		front->next = new_node;
		mem_tail = new_node;
	}
	new_node->mem_end = mem_end;
	new_node->mem_start = mem_end - n_bytes -1;
	if(new_node->mem_start < top) return NULL;
	
	start_addr_idx = (((uint32_t)(new_node->mem_start) & 0xFFC00000)>>22);
	end_addr_idx = (((uint32_t)(new_node->mem_end) & 0xFFC00000)>>22);
	for(i=start_addr_idx;i<=end_addr_idx;i++) {
		// printf("%d\n",i);
		page_directory[i].addr = ((i*FOURMEG) >> 22) & 0x3FF;
		page_directory[i].present = 1;
		page_directory[i].read_write = 1;
		page_directory[i].user_supervisor = 1;
		page_directory[i].page_size = 1;
	}
	// flush_tlb();
	memset(new_node->mem_start,0x0,n_bytes);
	flush_tlb();
	return  new_node->mem_start;	
}

void free(uint32_t ptr) {
	mem_list_t* head = mem_head;
	mem_list_t* front, *back;
	// printf("%d\n",ptr);
	// ptr = 0x1FBFFFEC;	
	if(head == NULL) return;
	if(head->mem_start == (mem_list_t*)ptr) {
		if(head->next == NULL) {
			mem_head = NULL;
			mem_tail = NULL;
			return;
		} else {
			mem_head = head->next;
		}
	} else {
		front = head;
		back = head->next;
		while(back != NULL) {
			if(back->mem_start == (mem_list_t*)ptr) {
				front->next = back->next;
				if(front->next == NULL) mem_tail = front;
			}
			front = front->next;
			back = back->next;
		}
	}
	return;
}

void pmap() {
	mem_list_t* head = mem_head;
	int i = 0;
	printf("-Index-----Start Address-------End Address-----Size-\n");
	while(head != NULL) {
		i++;
		if(i>9) printf(" %d:       0x%x           0x%x       %d \n",i,head->mem_start,head->mem_end,head->mem_end - head->mem_start-1);
		else printf("  %d:       0x%x           0x%x       %d \n",i,head->mem_start,head->mem_end,head->mem_end - head->mem_start-1);
		head = head->next;
	}
}

