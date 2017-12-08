#include "file.h"
// What type of filesystem are we developing? And do we need it for checkpoint2?
static boot_t boot_block;
static dentry_t * get_entry;
static const void * starter;
int index=0;
/*
*input: start_addr - locationt to initiatie
*output: 0 - sucess; initiation
*description: file system initiation
*/
int32_t file_init(const void * start_addr) {
	int i;
	starter=start_addr;
	memcpy(&boot_block.num_dentries,start_addr,ONE_BYTE);
	memcpy(&boot_block.num_inodes,start_addr+ONE_BYTE,ONE_BYTE);
	memcpy(&boot_block.num_blocks,start_addr+TWO_BYTE,ONE_BYTE);
	for(i=0;i<boot_block.num_dentries;i++) {
		memcpy(&boot_block.dentries[i],start_addr+DENTRY_SIZE*(i+1),DENTRY_SIZE);
	}
	
	dir_ops.open = &dir_open;
	dir_ops.read = &dir_read;
	dir_ops.write = &dir_write;
	dir_ops.close = &dir_close;
	
	file_ops.open = &file_open;
	file_ops.read = &file_read;
	file_ops.write = &file_write;
	file_ops.close = &file_close;
	
	return 0;
}

/*
*input: none
*output: to print to terminal the next file in the directory
*description: call read_dentry_by_index here so that it reads through all directories
*/
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {
	if(index>=boot_block.num_dentries) return 0;
	int i;
	for(i=0;i<FILENAME_SIZE;i++)
		if(&boot_block.dentries[index].filename[i] == '\0') break;
	// terminal_write(0,&boot_block.dentries[index].filename,i);
	memcpy(buf,boot_block.dentries[index].filename,i);
	index++;
	return i;
}

/*
*input: none
*output: to print to terminal the next file in the directory
*description: call read_dentry_by_index here so that it reads through all directories
*/
int32_t dir_open(const uint8_t* filename){
	return 0;
}
/*
*input: none
*output: to print to terminal the next file in the directory
*description: call read_dentry_by_index here so that it reads through all directories
*/
int32_t dir_close(int32_t fd){
	index = 0;
	return 0;
}
/*
*input: none
*output: to print to terminal the next file in the directory
*description: call read_dentry_by_index here so that it reads through all directories
*/
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){
	return -1;
}

/*
*input: filename - file name string
*output: open the file/directory
*desciption: open the directory/file
*/
int32_t file_open(const uint8_t* filename) {
	if(read_dentry_by_name(filename,get_entry)!=0)// get_entry now has access to the dentry block that holds the file info 
		return -1;
	return 0;
}

/*
*input: fd - file descriptor
*output: close the file/directory
*desciption: close the directory/file
*/
int32_t file_close(int32_t fd) {
	return 0;
}

/*
*input: fd - file descriptor, nbytes - length to copy
*output: buf - output buffer
*desciption: read the file by calling a read helper
*/
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
	int val;
	if(fd == -1) {
		val = read_data(get_entry->inode_num, 0, buf, nbytes) ;
	} else {
		val = read_data(term[term_pit].pcb_live->file_desc[fd].inode, term[term_pit].pcb_live->file_desc[fd].file_pos , buf, nbytes);
		term[term_pit].pcb_live->file_desc[fd].file_pos += val;
	}
	return val;
}

/*
*input: fd - file descriptor, nbytes - length to copy
*output: invalid
*desciption: read-only fs
*/
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
	return -1;
}

void update_inode(int32_t fd) {
	term[term_pit].pcb_live->file_desc[fd].inode = get_entry->inode_num;
}
/*
*input: fname - pointer to the searching string
*output: dentry - output handler
*desciption: read directory entry by name
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
	int i;
	uint8_t* temp;
	for(i=0;i<boot_block.num_dentries;i++) {
		temp = boot_block.dentries[i].filename;
		if(compare(fname,temp) != -1) {
			get_entry = &(boot_block.dentries[i]);
			return 0;
		}
	}
	return -1;
}

/*
*input: index - dentry index;
*output: dentry - output handler
*desciption: read directory entry by index
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
	if(index > boot_block.num_dentries) return -1;

	get_entry = &(boot_block.dentries[index]);
	return 0;
}

/*
*input: inode - inode index ; offset - offset from the start of the block; length - length to copy
*output: buf - output handle
*desciption: helper function to read file (from data blocks)
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length ) {
	int i;
	inode_t* node = get_inode_addr(inode);			//get inode from inode_number
	int file_length = node->length;
	uint32_t block_offset = offset%FOURKILO;
	uint32_t block_index = offset/FOURKILO;
	
	if(offset >= file_length){
		return 0;
	}
	
	uint8_t* block_addr = get_block_addr(node->block[block_index]);
	
	for (i = 0 ; i < file_length-offset && i < length; i ++){
		memcpy(&buf[i], &block_addr[block_offset], 1);
		block_offset ++;
		if(block_offset >= FOURKILO){
			block_offset = 0;
			block_index++;
			block_addr = get_block_addr(node->block[block_index]);	
		}
	}
	
	return i;
}

/*
*input: a,b - pointer to string
*output: return -1 - false ; 0 - true
*desciption: helper function to compare strings, 0 - identical, 1 - different
*/
int32_t compare(const uint8_t* a,uint8_t* b) {
	int i;
	for(i=0;i<32;i++) {
		if(a[i]!=b[i]) return -1;
		if(a[i]=='\0') return 0;
	}
	if(a[i] == '\0' || a[i] == ' ' || b[i] == '\0' || b[i] == ' ') return 0; // for filenames that are not null terminated
	return -1;
}

/*
*input: block_num - data block index
*output: returns pointer to a data block
*desciption: helper function to get the address of the specific data block by data block index input
*/
uint8_t* get_block_addr(uint32_t block_num){
	return (uint8_t*)(starter + FOURKILO*(boot_block.num_inodes +1) + block_num*FOURKILO);
	
}

/*
*input: node_num - inode index
*output: returns pointer to a inode
*desciption: helper function to get the address of the specific inode
*/
inode_t* get_inode_addr(uint32_t node_num){
	return (inode_t*)(starter + (node_num+1)*FOURKILO);	
}

uint8_t* tab_compare(uint8_t* buf) {
	int i,j;
	uint8_t* temp;
	if(buf[0] == 0x20) return NULL;
	for(i=0;i<boot_block.num_dentries;i++) {
		temp = boot_block.dentries[i].filename;
		for(j=0;j<32;j++) {
			if(buf[j]==NULL) return temp;
			if(buf[j]!=temp[j]) break;			
		}
	}
	return NULL;
}





















