#include "mouse.h"

uint32_t left_clk_flag=0;
uint32_t left_release_flag=0;
uint32_t left_drag_flag=0;
// uint32_t right_clk_flag=0;
// uint32_t right_release_flag=0;
uint8_t mouse_cycle=0;
int8_t mouse_byte[3];
int8_t mouse_x_local=0;
int8_t mouse_y_local=0;
int8_t x=160;
int8_t y_add =0;
int8_t y=100;

void mouse_init() {
	uint8_t status;  //unsigned char

	//Enable the auxiliary mouse device
	outb(0xA8,MOUSE_COM_PORT);

	//Enable the interrupts
	outb(0x20,MOUSE_COM_PORT);
	status = (inb(MOUSE_DATA_PORT) | 2);
	outb(0x60,MOUSE_COM_PORT);
	outb(status,MOUSE_DATA_PORT);

	//Tell the mouse to use default settings
	mouse_write(0xF6);
	mouse_read();  //Acknowledge

	//Enable the mouse
	mouse_write(0xF4);
	mouse_read();  //Acknowledge

	return;
}

void mouse_write(uint8_t write_char) {
	 outb(0xD4, MOUSE_COM_PORT);
	 outb(write_char, MOUSE_DATA_PORT);
}

uint8_t mouse_read() {
	return inb(MOUSE_DATA_PORT);
}

void mouse_handler() {
	cli();
	if(	isModex == 0 || isPacman ==1 ){
		switch(mouse_cycle)
		{
		case 0:
			mouse_byte[0]=inb(MOUSE_DATA_PORT);
			mouse_cycle++;
			break;
		case 1:
			mouse_byte[1]=inb(MOUSE_DATA_PORT);
			mouse_cycle++;
			break;
		case 2:
			mouse_byte[2]=inb(MOUSE_DATA_PORT);
			mouse_cycle=0;
		}
		send_eoi(MOUSE_IRQ);
		sti();
		return;
	}
		
		
	switch(mouse_cycle)
	{
		case 0:
			mouse_byte[0]=inb(MOUSE_DATA_PORT);
			mouse_cycle++;
			break;
		case 1:
			mouse_byte[1]=inb(MOUSE_DATA_PORT);
			mouse_cycle++;
			break;
		case 2:
			mouse_byte[2]=inb(MOUSE_DATA_PORT);
			mouse_y_local=mouse_byte[0];
			mouse_x_local=mouse_byte[2];
			mouse_cycle=0;
			
			if(((mouse_byte[1]&0x1) == 0) && left_clk_flag == 1) {
				left_release_flag = 1;
			} else {
				left_release_flag = 0;
			}
			if((mouse_byte[1]&0x1) == 1) {
				left_clk_flag = 1;
			} else if((mouse_byte[1]&0x1) == 0) {
				left_clk_flag = 0;
			}			
			
			if(mouse_x_local > 20)
				mouse_x_local = 20;
			else if(mouse_x_local < -20)
				mouse_x_local = -20;
			
			if(mouse_y_local > 20)
				mouse_y_local = 20;
			else if(mouse_y_local < -20)
				mouse_y_local = -20;
			
			// if(left_drag_flag == 1) {
				// if(mouse_x_local > 7)
					// mouse_x_local = 7;
				// else if(mouse_x_local < -7)
					// mouse_x_local = -7;
				
				// if(mouse_y_local > 7)
					// mouse_y_local = 7;
				// else if(mouse_y_local < -7)
					// mouse_y_local = -7;
			// }
			
			// mouse_x += mouse_x_local;
			// mouse_y += mouse_y_local;
			
			
			
			draw_mouse(mouse_x_local,mouse_y_local);
			
			mouse_x += mouse_x_local;
			mouse_y += mouse_y_local;
			
			
			if(mouse_x < 0)
				mouse_x = 0;
			else if(mouse_x > 319)
				mouse_x = 319;
			if(mouse_y < 0) 
				mouse_y = 0;
			else if(mouse_y > 199) 
				mouse_y = 199;
			
			image_t * temp;
			temp = tail_img_ptr;
			temp->start_x = mouse_x;
			temp->start_y = 199-mouse_y;
			
			if(isModex == 0 || isPacman == 1)
				return;
			break;
	}
	
	send_eoi(MOUSE_IRQ);
	sti();
	return;
}



void draw_mouse(int32_t del_x,int32_t del_y){
	int first = 1;
	image_t * temp;
	uint8_t mouse_store_buffer[8*8] ;
	int i;
	
	if(left_release_flag == 1) {
		if(left_drag_flag == 1) {
			left_drag_flag = 0;
		} else {
			temp = tail_img_ptr->prev;
			while(temp!= NULL){
				if(mouse_x > temp->start_x && mouse_x < (temp->start_x + temp->width)
					&& (199-mouse_y) > temp->start_y && (199-mouse_y) < (temp->start_y + temp->height))
				{
					if(temp->id == 1) {// switch to text mode
						set_text_mode_3(0);
						int temp2 = term_live;
						term_live = 1;
						send_eoi(MOUSE_IRQ);
						context_switch(temp2,term_live);
						return;
					} else if(temp->id == 2) { // create window
						add_img();
					} else if(temp->id == 6) { // start pacman
						int temp2 = term_live;
						term_live = 2;
						isPacman = 1;
						send_eoi(MOUSE_IRQ);
						context_switch(temp2,term_live);
						return;
					} else if(temp->id == 4 || temp->id == 5 || temp->id == 8) {
						if(mouse_x < (temp->start_x+temp->width-2) && mouse_x > (temp->start_x+temp->width-10)
							&& (199-mouse_y) > (temp->start_y+2) && (199-mouse_y) < (temp->start_y+10))
						{
							close_window(temp->id);
						}
					} else if(temp->id == 7) {
						start_gif();
					} else if(temp->id == 9) {
						start_paint();
					} else if(temp->id == 10) {
						if(mouse_x < (temp->start_x+temp->width-2) && mouse_x > (temp->start_x+temp->width-10)
							&& (199-mouse_y) > (temp->start_y+2) && (199-mouse_y) < (temp->start_y+10))
						{
							close_window(temp->id);
						}
					}
					break;
				}
				temp = temp->prev;
			}
		}
	} else if (left_clk_flag == 1 ){
		temp = tail_img_ptr->prev;
		while(temp!= NULL){
			if(temp->id == 10 && mouse_x > temp->start_x+2 && mouse_x < (temp->start_x + temp->width -2)
				&& (199-mouse_y) > temp->start_y+10 && (199-mouse_y) < (temp->start_y + temp->height -2)) {
				draw_paint(mouse_x-temp->start_x,(199-mouse_y)-temp->start_y);
				first = 0;
				break;
			} else if(mouse_x > temp->start_x && mouse_x < (temp->start_x + temp->width)
				&& (199-mouse_y) > temp->start_y && (199-mouse_y) < (temp->start_y + temp->height))
			{
				// move_window(temp->id);
				if(first == 1) bring_to_front(temp->id);
				if(first == 1) move_window(temp->id, del_x,del_y);
				first = 0;
				break;
			}
			temp = temp->prev;
		}

	}
	else if (left_release_flag == 0 && left_clk_flag == 0){
		for(i = 0 ;i < 64; i ++){
			mouse_store_buffer[i] = 0;
		}
		temp = tail_img_ptr;
		temp->start_x = mouse_x;
		temp->start_y = 199-mouse_y;
		draw_save_full_block(temp->matrix_ptr, mouse_store_buffer, temp->start_x, temp->start_y, temp->width, temp->height, 1);
		
		show_screen();
		draw_save_full_block(mouse_store_buffer, NULL, temp->start_x, temp->start_y, temp->width, temp->height, 0);
		return;
	}
	int mouse_redraw = 0;
	temp = head_img_ptr;
	while(temp!= NULL){
		if(temp->next ==NULL){
			for(i = 0 ;i < 64; i ++){
				mouse_store_buffer[i] = 0;
			}
			temp->start_x = mouse_x;
			temp->start_y = 199-mouse_y;
			if(left_release_flag != 0 || left_clk_flag != 0){
				mouse_redraw = 1;
				draw_save_full_block(temp->matrix_ptr, mouse_store_buffer, temp->start_x, temp->start_y, temp->width, temp->height, 1);
			}
			break;
		}
		draw_full_block(temp->start_x, temp->start_y, temp->matrix_ptr, temp->width, temp->height);
		temp = temp->next;
	}
	show_screen();
	if(mouse_redraw == 1)
		draw_save_full_block(mouse_store_buffer, NULL, temp->start_x, temp->start_y, temp->width, temp->height, 0);

	return;
}






















