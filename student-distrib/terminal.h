#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "x86_desc.h"
#include "key.h"
#include "types.h"
#include "system.h"

#define ROW_MAX 80
#define COL_MAX 25
#define BUF_MAX 129
#define BUF_MAX_V 128

int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int key_to_ter(unsigned char c);
void clear_buf(void);
void terminal_init(void);
void tab_comp(void);
void hist_access(int mode);
void buf_move(int mode);

int32_t std_bad_open(const uint8_t* a);
int32_t std_bad_close(int32_t a);
int32_t std_bad_read(int32_t a, void* b, int32_t c);
int32_t std_bad_write(int32_t a, const void* b, int32_t c);

extern unsigned int term_live;
extern unsigned int term_pit;
#endif /* _TERMINAL_H */
