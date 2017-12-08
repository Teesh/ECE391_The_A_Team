#ifndef _FILE_H
#define _FILE_H

#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "types.h"
#include "system.h"

#define FILENAME_SIZE 32
#define ONE_BYTE 4
#define TWO_BYTE 8
#define DENTRY_SIZE 64


// directory entry struct
typedef struct dentry {
	uint8_t filename[32];
	uint32_t filetype;
	uint32_t inode_num;
	uint32_t reserved[6];
} dentry_t;

// boot block sctruct
typedef struct boot {
	uint32_t num_dentries;
	uint32_t num_inodes;
	uint32_t num_blocks;
	uint32_t reserved[13]; // do we need to define reserved?
	dentry_t dentries[63];
} boot_t;

// inode struct
typedef struct inode {
	uint32_t length;
	uint32_t block[1023];
} inode_t;

// syscalls
int32_t dir_open(const uint8_t* filename);
int32_t dir_close(int32_t fd);
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_init(const void * blockname);
int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_open(const uint8_t* filename);
int32_t dir_close(int32_t fd);
//int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t compare(const uint8_t* a,uint8_t* b);
uint8_t* tab_compare(uint8_t* buf);

uint8_t* get_block_addr(uint32_t block_num);
inode_t* get_inode_addr(uint32_t node_num);
void update_inode(int32_t fd);



#endif /* _FILE_H */
