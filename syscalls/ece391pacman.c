#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

#define STEP  2
#define STOP 0
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_RIGHT 3
#define KEY_LEFT 4
#define NULL 0

 struct pacman_t {
	int32_t pos_x;
	int32_t pos_x_in;
	int32_t pos_y;
	int32_t pos_y_in;
	int32_t step_x;
	int32_t step_x_in;
	int32_t step_y;
	int32_t step_y_in;
	int32_t direction;
	int32_t direction_in;
	int32_t maze_x;
	int32_t maze_y;

};

static struct pacman_t  pacman;
static struct pacman_t  ghost;


	unsigned char maze_matrix [25][28]={ 
	{2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2  },
	{2, 7, 6, 1, 1, 4, 7, 6, 1, 1, 1, 4, 7, 2, 2, 7, 6, 1, 1, 1, 4, 7, 6, 1, 1, 4, 7, 2  },
	{2, 7, 5, 1, 1, 3, 7, 5, 1, 1, 1, 3, 7, 5, 3, 7, 5, 1, 1, 1, 3, 7, 5, 1, 1, 3, 7, 2  },
	{2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2  },
	{2, 7, 6, 1, 1, 4, 7, 6, 4, 7, 6, 1, 1, 1, 1, 1, 1, 4, 7, 6, 4, 7, 6, 1, 1, 4, 7, 2  },
	{2, 7, 5, 1, 1, 3, 7, 2, 2, 7, 5, 1, 1, 4, 6, 1, 1, 3, 7, 2, 2, 7, 5, 1, 1, 3, 7, 2  },
	{2, 7, 7, 7, 7, 7, 7, 2, 2, 7, 7, 7, 7, 2, 2, 7, 7, 7, 7, 2, 2, 7, 7, 7, 7, 7, 7, 2  },
	{5, 1, 1, 1, 1, 4, 7, 2, 5, 1, 1, 4, 0, 2, 2, 0, 6, 1, 1, 3, 2, 7, 6, 1, 1, 1, 1, 3  },
	{0, 0, 0, 0, 0, 2, 7, 2, 6, 1, 1, 3, 0, 5, 3, 0, 5, 1, 1, 4, 2, 7, 2, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 2, 7, 5, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 3, 7, 2, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 2, 7, 0, 0, 0, 6, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 7, 2, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 2, 7, 0, 0, 0, 5, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 7, 2, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 2, 7, 6, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 4, 7, 2, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 2, 7, 2, 2, 0, 6, 1, 1, 1, 1, 1, 1, 4, 0, 2, 2, 7, 2, 0, 0, 0, 0, 0  },
	{6, 1, 1, 1, 1, 3, 7, 5, 3, 0, 5, 1, 1, 4, 6, 1, 1, 3, 0, 5, 3, 7, 5, 1, 1, 1, 1, 4  },
	{2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2  },
	{2, 7, 6, 1, 1, 4, 7, 6, 1, 1, 1, 4, 7, 2, 2, 7, 6, 1, 1, 1, 4, 7, 6, 1, 1, 4, 7, 2  },
	{2, 7, 5, 1, 4, 2, 7, 5, 1, 1, 1, 3, 7, 5, 3, 7, 5, 1, 1, 1, 3, 7, 2, 6, 1, 3, 7, 2  },
	{2, 7, 7, 7, 2, 2, 7, 7, 7, 7, 7, 7, 7, 0, 0, 7, 7, 7, 7, 7, 7, 7, 2, 2, 7, 7, 7, 2  },
	{5, 1, 4, 7, 2, 2, 7, 6, 4, 7, 6, 1, 1, 1, 1, 1, 1, 4, 7, 6, 4, 7, 2, 2, 7, 6, 1, 3  },
	{6, 1, 3, 7, 5, 3, 7, 2, 2, 7, 5, 1, 1, 4, 6, 1, 1, 3, 7, 2, 2, 7, 5, 3, 7, 5, 1, 4  },
	{2, 7, 7, 7, 7, 7, 7, 2, 2, 7, 7, 7, 7, 2, 2, 7, 7, 7, 7, 2, 2, 7, 7, 7, 7, 7, 7, 2  },
	{2, 7, 6, 1, 1, 1, 1, 3, 5, 1, 1, 4, 7, 2, 2, 7, 6, 1, 1, 3, 5, 1, 1, 1, 1, 4, 7, 2  },
	{2, 7, 5, 1, 1, 1, 1, 1, 1, 1, 1, 3, 7, 5, 3, 7, 5, 1, 1, 1, 1, 1, 1, 1, 1, 3, 7, 2  },
	{2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2  }
	};

	unsigned char bean_matrix [25][28]={ 
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0  },
	{0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0  },
	{0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0  },
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0  },
	{0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0  },
	{0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0  },
	{0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0  },
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0  },
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0  },
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0  },
	{0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0  },
	{0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0  },
	{0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0  },
	{0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0  },
	{0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0  },
	{0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0  },
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0  },
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0  },
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0  }
	};
void load_maze(){
	ece391_pac(NULL,8);
	
	int w = 28;
	int h = 25;
	int x,y;
	int wall_type;

	for(y = 0 ; y < h ; y++){
		for(x = 0; x < w; x ++){
			wall_type = maze_matrix[y][x];	
			pacman.maze_x = x;
			pacman.maze_y = y;
			switch (wall_type){
			case 0:
				ece391_pac(&pacman,0);
				break;
			case 1:
				ece391_pac(&pacman,1);
				break;
			case 2:
				ece391_pac(&pacman,2);
				break;
			case 3:
				ece391_pac(&pacman,3);
				break;
			case 4:
				ece391_pac(&pacman,4);
				break;
			case 5:
				ece391_pac(&pacman,5);
				break;
			case 6:
				ece391_pac(&pacman,6);
				break;
			case 7:
				ece391_pac(&pacman,7);
				break;
			}	
		}
	}
	ece391_pac(NULL,9);

}

uint8_t canMove(int x, int y, int dir){
	int temp_x = 0;
	int temp_y = 0;
	int ret = 0;
		
	
	switch (dir){
	case KEY_UP:
			temp_y = -4; // 9 = 3+6
		break;
	case KEY_DOWN:
			temp_y = 10;
		break;
	case KEY_LEFT:
			 temp_x = -4;
		break;
	case KEY_RIGHT:
			temp_x = 10;
		break;	
	}
	
	if(x + temp_x <0 || x+temp_x>320)
		return 0;
	
	if(y + temp_y <0 || y+temp_y +2 >200)
		return 0;
	
	if((maze_matrix[(y+ temp_y )/8][(x+temp_x)/8] != 0) && (maze_matrix[(y+ temp_y)/8][(x+temp_x)/8] != 7))
		return 0;
	return 1;
}

void find_pacman_next_position( int last_key){
	switch (last_key){
	case KEY_UP:
			if (canMove(pacman.pos_x, pacman.pos_y, KEY_UP)){
				if (pacman.step_x == 0){
					pacman.step_x_in  = 0;
					pacman.step_y_in  = -STEP;
				}	
				else {
					if( ( (pacman.step_x == -STEP) &&  ((pacman.pos_x - STEP) % 8 == 0)) || (pacman.step_x == 0 && pacman.step_y == 0) ){
							pacman.step_x_in  = 0;
							pacman.step_y_in  = -STEP;
					}
					else if((pacman.step_x == STEP) &&  ((pacman.pos_x + STEP) % 8 == 0) ){
							pacman.step_x_in  = 0;
							pacman.step_y_in  = -STEP;
					}
					else{
						pacman.step_x_in  = pacman.step_x;
						pacman.step_y_in  = 0 ;
					}
				}					
			}
			else{
				pacman.step_y_in = 0;
				pacman.step_x_in = 0;
				if (pacman.step_x == STEP){
					if( canMove(pacman.pos_x, pacman.pos_y, KEY_RIGHT))
						pacman.step_x_in = STEP;
				}
				else if(pacman.step_x == -STEP){
					if( canMove(pacman.pos_x, pacman.pos_y, KEY_LEFT))
						pacman.step_x_in = -STEP;
				}
			}
		break;
	case KEY_DOWN:
			if (canMove(pacman.pos_x, pacman.pos_y, KEY_DOWN)){
				if (pacman.step_x == 0){
					pacman.step_x_in  = 0;
					pacman.step_y_in  = STEP;
				}	
				else {
					if(((pacman.step_x == -STEP) &&  ((pacman.pos_x - STEP) % 8 == 0))   || (pacman.step_x == 0 && pacman.step_y == 0)      ){
							pacman.step_x_in  = 0;
							pacman.step_y_in  = STEP;
					}
					else if((pacman.step_x == STEP) &&  ((pacman.pos_x + STEP) % 8 == 0) ){
							pacman.step_x_in  = 0;
							pacman.step_y_in  = STEP;
					}
					else{
						pacman.step_x_in  = pacman.step_x;
						pacman.step_y_in  = 0 ;
					}
				}					
			}
			else{
				pacman.step_y_in = 0;
				pacman.step_x_in = 0;
				if (pacman.step_x == STEP){
					if( canMove(pacman.pos_x, pacman.pos_y, KEY_RIGHT))
						pacman.step_x_in = STEP;
				}
				else if(pacman.step_x == -STEP){
					if( canMove(pacman.pos_x, pacman.pos_y, KEY_LEFT))
						pacman.step_x_in = -STEP;
				}
			}
		break;
	case KEY_LEFT:
			if (canMove(pacman.pos_x, pacman.pos_y, KEY_LEFT)){
				if (pacman.step_y == 0){
					pacman.step_x_in  = -STEP;
					pacman.step_y_in  = 0;
				}	
				else {
					if( ((pacman.step_y == -STEP) &&  ((pacman.pos_y - STEP) % 8 == 0)) || (pacman.step_x == 0 && pacman.step_y == 0)  ){
							pacman.step_x_in  = -STEP;
							pacman.step_y_in  = 0;
					}
					else if((pacman.step_y == STEP) &&  ((pacman.pos_y + STEP) % 8 == 0) ){
							pacman.step_x_in  = -STEP;
							pacman.step_y_in  = 0;
					}
					else{
						pacman.step_x_in  = 0;
						pacman.step_y_in  = pacman.step_y;
					}
				}					
			}
			else{
				pacman.step_y_in = 0;
				pacman.step_x_in = 0;
				if (pacman.step_y == STEP){
					if( canMove(pacman.pos_x, pacman.pos_y, KEY_DOWN))
						pacman.step_y_in = STEP;
				}
				else if(pacman.step_y == -STEP){
					if( canMove(pacman.pos_x, pacman.pos_y, KEY_UP))
						pacman.step_y_in = -STEP;
				}
			}
		break;
	case KEY_RIGHT:
			if (canMove(pacman.pos_x, pacman.pos_y, KEY_RIGHT)){
				if (pacman.step_y == 0){
					pacman.step_x_in  = STEP;
					pacman.step_y_in  = 0;
				}	
				else {
					if( ((pacman.step_y == -STEP) &&  ((pacman.pos_y - STEP) % 8 == 0))     || (pacman.step_x == 0 && pacman.step_y == 0)  ){
							pacman.step_x_in  = -STEP;
							pacman.step_y_in  = 0;
					}
					else if((pacman.step_y == STEP) &&  ((pacman.pos_y + STEP) % 8 == 0) ){
							pacman.step_x_in  = -STEP;
							pacman.step_y_in  = 0;
					}
					else{
						pacman.step_x_in  = 0;
						pacman.step_y_in  = pacman.step_y;
					}
				}					
			}
			else{
				pacman.step_y_in = 0;
				pacman.step_x_in = 0;
				if (pacman.step_y == STEP){
					if( canMove(pacman.pos_x, pacman.pos_y, KEY_DOWN))
						pacman.step_y_in = STEP;
				}
				else if(pacman.step_y == -STEP){
					if( canMove(pacman.pos_x, pacman.pos_y, KEY_UP))
						pacman.step_y_in = -STEP;
				}
			}
		break;
	}
	
	
	if(pacman.step_y_in == 0){
		if(pacman.step_x_in == STEP)
			pacman.direction_in = KEY_RIGHT;
		else if(pacman.step_x_in == -STEP)
			pacman.direction_in = KEY_LEFT;
		else 
			pacman.direction_in = pacman.direction;
	}
	else if(pacman.step_y_in == STEP)
		pacman.direction_in = KEY_DOWN;
	else 
		pacman.direction_in = KEY_UP;	

	pacman.pos_x_in = pacman.pos_x + pacman.step_x;
	pacman.pos_y_in = pacman.pos_y + pacman.step_y;
}




void find_ghost_next_position(){
	int left_dist = -1, right_dist = -1, up_dist =-1 , down_dist = -1;
	int pac_x = pacman.pos_x;
	int pac_y = pacman.pos_y;
	
	
	if(canMove(ghost.pos_x, ghost.pos_y, KEY_UP)) {
		int temp_x = ghost.pos_x ;
		int temp_y = ghost.pos_y - STEP;
		up_dist = (temp_x - pac_x )* (temp_x - pac_x )  + (temp_y - pac_y) * (temp_y - pac_y);	
	}
	if(canMove(ghost.pos_x, ghost.pos_y, KEY_DOWN)) {
		int temp_x = ghost.pos_x ;
		int temp_y = ghost.pos_y + STEP;
		down_dist = (temp_x - pac_x )* (temp_x - pac_x )  + (temp_y - pac_y) * (temp_y - pac_y);	
	}
	if(canMove(ghost.pos_x, ghost.pos_y, KEY_RIGHT)) {
		int temp_x = ghost.pos_x + STEP;
		int temp_y = ghost.pos_y ;
		right_dist = (temp_x - pac_x )* (temp_x - pac_x )  + (temp_y - pac_y) * (temp_y - pac_y);	
	}
	
	if(canMove(ghost.pos_x, ghost.pos_y, KEY_LEFT)) {
		int temp_x = ghost.pos_x - STEP;
		int temp_y = ghost.pos_y ;
		left_dist = (temp_x - pac_x )* (temp_x - pac_x )  + (temp_y - pac_y) * (temp_y - pac_y);	
	}
	
	int min = 320*200;
	ghost.step_x_in = 0;
	ghost.step_y_in = 0;
	if (left_dist <=  min && left_dist >=0 && ghost.step_x != STEP){
		min = left_dist;
		ghost.step_y_in = 0;
		ghost.step_x_in = -STEP;
	}
	if (right_dist <= min && right_dist >=0  && ghost.step_x != -STEP){
		min = right_dist;
		ghost.step_x_in = STEP;
		ghost.step_y_in = 0;
	}
	if (up_dist <= min && up_dist >=0    && ghost.step_y != STEP      ){
		min = up_dist;
		ghost.step_y_in = -STEP;
		ghost.step_x_in = 0;
	}
	if (down_dist <= min && down_dist >=0      && ghost.step_y != -STEP ){
		min = down_dist;
		ghost.step_y_in = STEP;
		ghost.step_x_in = 0;
	}
	
	
	ghost.pos_x_in = ghost.pos_x + ghost.step_x;
	ghost.pos_y_in = ghost.pos_y + ghost.step_y;
}




void gotoLose(){
	ece391_pac(NULL,26);
	while(1);
}
void gotoWin(){
	ece391_pac(NULL,27);
	while(1);
}

void start_pacman(){	
    int garbage;
	uint32_t last_key;
	int rtc_fd = ece391_open((uint8_t*)"rtc");

	int COUNT_WIN = 232;
	int count = 0;
	int MIN_DIS = 6;
	int counter_stop = 0;
	int COUNT_STOP_MAX = 96;
	uint32_t count_rtc = 0;

	
	while(1){
		int temp_x = pacman.pos_x - ghost.pos_x;
		int temp_y = pacman.pos_y - ghost.pos_y;
		if(temp_x < MIN_DIS && temp_x > -MIN_DIS && temp_y < MIN_DIS && temp_y > - MIN_DIS){
			gotoLose();
			break;
		}
		if(count == COUNT_WIN){
			gotoWin();
		}
		
		ece391_read(rtc_fd, &garbage, 4);
		count_rtc++;
		if(count_rtc%16 != 0) continue;
		
		uint8_t redraw_pac = 0, redraw_ghost=0;
		last_key = ece391_pac(NULL,50);
		
		find_pacman_next_position( last_key);
		find_ghost_next_position();

		pacman.direction = pacman.direction_in;
		pacman.step_x = pacman.step_x_in;
		pacman.step_y = pacman.step_y_in;
		ghost.step_x = ghost.step_x_in;
		ghost.step_y = ghost.step_y_in;
		

		
		if(pacman.pos_x != pacman.pos_x_in || pacman.pos_y != pacman.pos_y_in){
			counter_stop = 0;
						
			ece391_pac(&pacman,25);
			ece391_pac(&pacman,10);

			switch (pacman.direction){
				case KEY_UP:
						ece391_pac(&pacman,11);
					break;
				case KEY_DOWN:
						ece391_pac(&pacman,12);
					break;
				case KEY_RIGHT:
						ece391_pac(&pacman,13);
						break;
				case KEY_LEFT:
						ece391_pac(&pacman,14);
					break;

			}
			if(bean_matrix[pacman.pos_y /8][pacman.pos_x /8] == 0 )
				ece391_pac(&pacman,15);
			else
				ece391_pac(&pacman,16);
			
			redraw_pac = 1;
			switch (pacman.direction){
				case KEY_UP:
					ece391_pac(&pacman,17);
					break;
				case KEY_DOWN:
						ece391_pac(&pacman,18);
					break;
				case KEY_RIGHT:
						ece391_pac(&pacman,19);
						break;
				case KEY_LEFT:
						ece391_pac(&pacman,20);
					break;

			}
		}
		else {
			counter_stop ++;
		}
		

		if(bean_matrix[pacman.pos_y /8][pacman.pos_x /8] == 1 ){
			count ++;
			bean_matrix[pacman.pos_y /8][pacman.pos_x /8] = 0 ;
		}
		

		
		
		if( (ghost.pos_x != ghost.pos_x_in) || (ghost.pos_y != ghost.pos_y_in) ){
			ece391_pac(&ghost,21);
			ece391_pac(&ghost,22);
			ece391_pac(&ghost,23);
			ece391_pac(&ghost,24);
		}
		
		ghost.pos_x = ghost.pos_x_in;
		ghost.pos_y = ghost.pos_y_in;
		
		
		if(counter_stop >  COUNT_STOP_MAX){
			if(bean_matrix[pacman.pos_y /8][pacman.pos_x /8] == 0 )
				ece391_pac(&pacman,15);
			else
				ece391_pac(&pacman,16);
			ece391_pac(&pacman,25);
			pacman.pos_x = 110;
			pacman.pos_y = 144;
			pacman.step_x = 0;
			pacman.step_x_in = 0;
			pacman.step_y = 0;
			pacman.step_x_in = 0;
			pacman.pos_x_in = 110;
			pacman.pos_y_in = 144;
			pacman.direction = KEY_UP;
			pacman.direction_in = KEY_UP;

			ece391_pac(&pacman,25);
			ece391_pac(&pacman,11);
			
			if(bean_matrix[pacman.pos_y /8][pacman.pos_x /8] == 0 )
				ece391_pac(&pacman,15);
			else
				ece391_pac(&pacman,16);
			
			ece391_pac(&pacman,17);
			counter_stop = 0;
		}
		else{
			pacman.pos_x = pacman.pos_x_in;
			pacman.pos_y = pacman.pos_y_in;
		}
	}
}


int main(){
	load_maze();
	pacman.step_x = 0;
	pacman.step_y = 0;
	pacman.step_x_in = 0;
	pacman.step_y_in = 0;
	pacman.direction = KEY_UP;
	pacman.pos_x = 110;
	// pacman.pos_x_in = 110;
	pacman.pos_y = 144;
	// pacman.pos_y_in = 142;
	
	
	ghost.step_x = 0;
	ghost.step_y = 0;
	ghost.step_x_in = 0;
	ghost.step_y_in = 0;
	ghost.pos_x = 110 ;
	ghost.pos_y = 72 ;
	
	
	// int rate = 32;
	// rtc_write(0,&rate,0);

	start_pacman();
	return 0;
}



