#include "terminal.h"

#define NUM_COLS 80
#define NUM_ROWS 25

uint32_t buf_index[3] = {0,0,0};
uint32_t buf_index_max[3] = {0,0,0};
char key_buf[3][BUF_MAX];
char store_buf[3][BUF_MAX];
uint32_t hist_idx[3] = {0,0,0};
uint32_t arrow_idx[3] = {0,0,0};
uint32_t hist_max = 0;
char history[3][32][BUF_MAX];
volatile uint32_t enter_flag[3] = {0,0,0};
unsigned int term_live = 0;
unsigned int term_pit = 0;

void terminal_init(void){
	stdin_ops.open = &std_bad_open;
	stdin_ops.read = &terminal_read;
	stdin_ops.write = &std_bad_write;
	stdin_ops.close = &std_bad_close;

	stdout_ops.open = &std_bad_open;
	stdout_ops.read = &std_bad_read;
	stdout_ops.write = &terminal_write;
	stdout_ops.close = &std_bad_close;
}

/*
*input: none
*output: always -1
*desciption: sets stdin and stdout ops which can't be used
*/
int32_t std_bad_open(const uint8_t* a) {
	return -1;
}
int32_t std_bad_close(int32_t a) {
	return -1;
}
int32_t std_bad_read(int32_t a, void* b, int32_t c) {
	return -1;
}
int32_t std_bad_write(int32_t a, const void* b, int32_t c) {
	return -1;
}

/*
*input: filename
*output: always 0
*desciption: open on a terminal does nothing
*/
int32_t terminal_open(const uint8_t* filename) {
	return 0; // do nothing on read
}

/*
*input: fd - file descriptor
*output: always -1
*desciption: returns fail because we can't close a terminal
*/
int32_t terminal_close(int32_t fd) {
	return -1; // can't close terminal
}

/*
*input: fd - file descriptor, buf - pointer to a buffer than needs filling, nbytes - doesn't do shit in this function
*output: sets buffer to keyboard buffer, returns number of chars in buffer
*desciption: return the keyboard buffer and number of chars when we hit enter
*/
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
	enter_flag[term_pit] = 0;
	while(!enter_flag[term_pit]);
	memcpy(buf,store_buf[term_pit],enter_flag[term_pit]);
	return enter_flag[term_pit];
}

/*
*input: fd - file descriptor, buf - buffer of chars to write, nbytes - number of bytes to write
*output: i - number of bytes read
*desciption: write the buffer to the terminal
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
	int i;
	const char* char_buf = buf;
	for(i = 0;i < nbytes;i++) {
		ter_write_pit(char_buf[i]);
	}
	return i;
}

/*
*input: c - character to write
*output: none we care about
*desciption: writes a char to terminal from keyboard, but also handles special cases and keys
*/
int key_to_ter(unsigned char c) {
	int i,diff;
	if(c == '\b') {
		if(buf_index[term_live] == 0) {
			return 0;
		} else {
			buf_index_max[term_live]--;
			buf_index[term_live]--;
			key_buf[term_live][buf_index[term_live]] = '\0';
			ter_write('\b',buf_index[term_live],buf_index_max[term_live]);
			for(i=0;i<buf_index_max[term_live]-buf_index[term_live];i++)
				key_buf[term_live][buf_index[term_live]+i] = key_buf[term_live][buf_index[term_live]+i+1];
			key_buf[term_live][buf_index_max[term_live]] = '\0';
			buf_index[term_live]--;
			buf_index_max[term_live]--;
		}
	} else if(c == '\n' || c == '\r') {
		diff = buf_index_max[term_live] - buf_index[term_live];
		if(diff != 0) {
			term[term_live].screen_x = (term[term_live].screen_x+diff)%NUM_COLS;
			if(buf_index_max[term_live] > 72) term[term_live].screen_y++;
		}
		key_buf[term_live][buf_index_max[term_live]] = c;
		ter_write(key_buf[term_live][buf_index_max[term_live]],buf_index[term_live],buf_index_max[term_live]);
		memcpy(store_buf[term_live],key_buf[term_live],BUF_MAX);
		if(key_buf[term_live][0] != '\0' && key_buf[term_live][0] != '\n') {
			memcpy(history[term_live][hist_idx[term_live]],key_buf[term_live],BUF_MAX);
			hist_idx[term_live] = (hist_idx[term_live]+1)%32;
			arrow_idx[term_live] = hist_idx[term_live];
			if(hist_max < 32) hist_max++;
			for(i=0;i<BUF_MAX;i++) {
				history[term_live][arrow_idx[term_live]][i]='\0';
			}
		}
		enter_flag[term_live] = buf_index_max[term_live]+1;
		clear_buf();
	} else if(buf_index_max[term_live] > BUF_MAX_V) {
		return 0;
	} else {
		for(i=buf_index_max[term_live]-buf_index[term_live];i>=0;i--)
			key_buf[term_live][buf_index[term_live]+i+1] = key_buf[term_live][buf_index[term_live]+i];
		key_buf[term_live][buf_index[term_live]] = c;
		ter_write(key_buf[term_live][buf_index[term_live]],buf_index[term_live],buf_index_max[term_live]);
	}
	if(!(c == '\n' || c == '\r')) { // prevent being able to backspace into previous line after 
		buf_index[term_live]++;
		buf_index_max[term_live]++;
	}
	return 0;
}


/*
*input: none
*output: none
*desciption: clear the buffer
*/
void clear_buf() {
	int i;
	buf_index[term_live] = 0;
	buf_index_max[term_live] = 0;
	for(i=0;i<BUF_MAX_V;i++) {
		key_buf[term_live][i] = 0;
	}
	key_buf[term_live][BUF_MAX_V] = '\n';
}

void tab_comp() {
	uint8_t* temp;
	int i,part,size,index;
	for(index=buf_index_max[term_live];index>0;index--) {
		if(key_buf[term_live][index-1] == 0x20) break;
	}
	if((temp = tab_compare((uint8_t*)&key_buf[term_live][index])) == NULL) return;
	for(size=0;size<32;size++) {
		if(temp[size]=='\0') break;
	}
	for(part=0;key_buf[term_live][index+part]!=NULL && key_buf[term_live][index+part]!='\n';part++);
	for(i=part;i<size;i++) {
		key_to_ter(temp[i]);
	}
	return;
}

void hist_access(int mode) {
	int i;
	if(mode) {
		if(history[term_live][(arrow_idx[term_live]-1)%32][0] == '\0') return;
		arrow_idx[term_live] = (arrow_idx[term_live]-1)%32;
		for(i=0;i<BUF_MAX;i++)
			key_to_ter('\b');
		for(i=0;i<BUF_MAX;i++) {
			if(history[term_live][arrow_idx[term_live]][i] == '\0' || history[term_live][arrow_idx[term_live]][i] == '\n') break;
			key_to_ter(history[term_live][arrow_idx[term_live]][i]);
		}
		return;
	} else {
		if(arrow_idx[term_live] == hist_idx[term_live]) return;
		if(history[term_live][(arrow_idx[term_live]+1)%32][0] == '\0') {
			if(history[term_live][(arrow_idx[term_live])%32][0] != '\0')
				arrow_idx[term_live] = (arrow_idx[term_live]+1)%32;
			for(i=0;i<BUF_MAX;i++)
				key_to_ter('\b');
			return;
		}
		arrow_idx[term_live] = (arrow_idx[term_live]+1)%32;
		for(i=0;i<BUF_MAX;i++)
			key_to_ter('\b');
		for(i=0;i<BUF_MAX;i++) {
			if(history[term_live][arrow_idx[term_live]][i] == '\0' || history[term_live][arrow_idx[term_live]][i] == '\n') break;
			key_to_ter(history[term_live][arrow_idx[term_live]][i]);
		}
	}
	return;
}

void buf_move(int mode) {
	if(mode) {
		if(buf_index[term_live] == 0) return;
		buf_index[term_live]--;
		if(term[term_live].screen_x != 0) {
			term[term_live].screen_x--;
		} else {
			term[term_live].screen_x = NUM_COLS-1;
			term[term_live].screen_y--;
		}
		cursor(term[term_live].screen_x,term[term_live].screen_y);
	} else {
		if(buf_index[term_live] >= buf_index_max[term_live]) return;
		buf_index[term_live]++;
		if(term[term_live].screen_x != NUM_COLS-1) {
			term[term_live].screen_x++;
		} else {
			term[term_live].screen_x = 0;
			term[term_live].screen_y++;
		}
		cursor(term[term_live].screen_x,term[term_live].screen_y);
	}
}

