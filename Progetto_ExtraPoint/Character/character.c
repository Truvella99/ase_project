#include "LPC17xx.h"
#include "../GLCD/GLCD.h" 
#include "character.h"
#include "../timer/timer.h"
#include <stdio.h>
#include "../RIT/RIT.h"
//SHARED VARIABLES
uint8_t battery_cell_1 = 0, battery_cell_2 = 0;
uint16_t character_x = 75, character_y = 130;
uint8_t frame = 0;
uint8_t is_shifting = 0;
uint8_t is_flipping = 0;
uint8_t joystick_selection = 0;
uint8_t selection_disabled = 0;
uint8_t joystick_disabled = 0;
uint8_t reset = 0;
uint8_t select_pressed = 0;
int time_elapsed = 0;
int volume = 0;
uint8_t is_animating = 0;

// 1 = draw square 0 = not draw square
const uint8_t first_frame_character[15][13] = {
		0,0,1,1,0,0,0,0,0,1,1,0,0,
		0,1,1,1,1,0,0,0,1,1,1,1,0,
		0,1,1,1,1,1,1,1,1,1,1,1,0,
		0,1,1,0,0,0,0,0,0,0,1,1,0,
		0,1,0,0,1,0,0,0,1,0,0,1,0,
		1,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,1,1,1,0,0,0,0,1,
		0,1,0,0,0,0,0,0,0,0,0,1,0,
		0,1,0,1,0,0,0,0,0,1,0,0,1,
		0,1,1,1,0,0,0,0,0,0,1,1,0,
		0,1,0,0,0,0,0,0,0,0,0,1,0,
		0,0,1,0,0,0,0,0,0,0,1,0,0,
		0,0,0,1,0,1,1,1,0,1,0,0,0,
		0,0,0,0,1,0,0,1,0,1,0,0,0,
		0,0,0,0,0,0,0,0,1,0,0,0,0
};

// Square to clear (row/column indexes related to character matrix to clear) when changing frame
const uint8_t clear[14] = {
		8,1,8,12,
		9,3,
		13,4,13,7,13,9,
		14,8
};

// Square to fill (row/column indexes related to character matrix to fill) when changing frame
const uint8_t fill[14] = {
		8,0,8,11,
		9,9,
		13,3,13,5,13,8,
		14,4
};

// Square to clear (row/column indexes related to old (before sx shift) position of character first frame to clear)
const uint8_t first_clear_sx[92] = {
		0,2,0,3,0,9,0,10,
		1,2,1,3,1,4,1,9,1,10,1,11,
		2,9,2,10,2,11,
		3,1,3,2,3,10,3,11,
		4,4,4,11,
		5,0,5,12,
		6,0,6,5,6,6,6,7,6,12,
		7,1,7,11,
		8,1,8,3,8,12,
		9,1,9,2,9,3,9,10,9,11,
		10,1,10,11,
		11,2,11,10,
		12,5,12,7,12,9,
		13,7,13,9,
		14,8
};

// Square to clear (row/column indexes related to old (before sx shift) position of character second frame to clear)
const uint8_t second_clear_sx[92] = {
		0,2,0,3,0,9,0,10,
		1,2,1,3,1,4,1,9,1,10,1,11,
		2,9,2,10,2,11,
		3,1,3,2,3,10,3,11,
		4,4,4,11,
		5,0,5,12,
		6,0,6,5,6,6,6,7,6,12,
		7,1,7,11,
		8,3,8,9,8,11,
		9,1,9,2,9,9,9,10,9,11,
		10,1,10,11,
		11,2,11,10,
		12,5,12,7,12,9,
		13,3,13,8,
		14,4
};

// Square to fill (row/column indexes related to new (after sx shift) position of character first frame to fill)
const uint8_t first_fill_sx[92] = {
		0,2,0,3,0,9,0,10,
		1,1,1,2,1,3,1,8,1,9,1,10,
		2,1,2,2,2,3,
		3,1,3,2,3,10,3,11,
		4,1,4,8,
		5,0,5,12,
		6,0,6,5,6,6,6,7,6,12,
		7,1,7,11,
		8,1,8,3,8,9,
		9,1,9,2,9,3,9,10,9,11,
		10,1,10,11,
		11,2,11,10,
		12,3,12,5,12,7,
		13,4,13,9,
		14,8
};

// Square to fill (row/column indexes related to new (after sx shift) position of character second frame to fill)
const uint8_t second_fill_sx[92] = {
		0,2,0,3,0,9,0,10,
		1,1,1,2,1,3,1,8,1,9,1,10,
		2,1,2,2,2,3,
		3,1,3,2,3,10,3,11,
		4,1,4,8,
		5,0,5,12,
		6,0,6,5,6,6,6,7,6,12,
		7,1,7,11,
		8,0,8,9,8,11,
		9,1,9,2,9,9,9,10,9,11,
		10,1,10,11,
		11,2,11,10,
		12,3,12,5,12,7,
		13,3,13,5,
		14,4
};

const uint8_t heart [6][7]= {
    0,1,1,0,1,1,0,
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,
    0,1,1,1,1,1,0,
    0,0,1,1,1,0,0,
    0,0,0,1,0,0,0
};

const uint8_t speaker [17][13]= {
    0,0,0,0,0,0,0,0,0,0,0,1,1,
    0,0,0,0,0,0,0,0,0,0,1,1,1,
    0,0,0,0,0,0,0,0,0,1,1,1,1,
    0,0,0,0,0,0,0,0,1,1,1,1,1,
    0,0,0,0,0,0,0,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,1,1
};

const uint8_t sound [12][8]= {
    0,0,0,0,0,1,0,0,
    0,0,1,0,0,0,1,0,
    0,0,0,1,0,0,1,0,
    1,0,0,0,1,0,0,1,
    0,1,0,0,1,0,0,1,
    0,1,0,0,1,0,0,1,
    0,1,0,0,1,0,0,1,
    0,1,0,0,1,0,0,1,
    1,0,0,0,1,0,0,1,
    0,0,0,1,0,0,1,0,
    0,0,1,0,0,0,1,0,
    0,0,0,0,0,1,0,0
};

const uint8_t mute_cross [10][10]= {
    1,1,0,0,0,0,0,0,1,1,
    1,1,0,0,0,0,0,0,1,1,
    0,0,1,1,0,0,1,1,0,0,
    0,0,1,1,0,0,1,1,0,0,
    0,0,0,0,1,1,0,0,0,0,
    0,0,0,0,1,1,0,0,0,0,
    0,0,1,1,0,0,1,1,0,0,
    0,0,1,1,0,0,1,1,0,0,
    1,1,0,0,0,0,0,0,1,1,
    1,1,0,0,0,0,0,0,1,1
};

void draw_heart(uint8_t x, uint8_t y, uint16_t color) {
		uint8_t column = 0;
		uint8_t row = 0;
		uint8_t initial_x_coordinate = x;
		uint8_t initial_y_coordinate = y;
		for (row=0;row<6;row++) {
			for(column=0;column<7;column++) {
					if(heart[row][column]==1) {
						LCD_DrawRectangle(initial_x_coordinate+(column*BLOCK_WIDTH_SMALL),initial_y_coordinate+(row*BLOCK_HEIGHT_SMALL),BLOCK_WIDTH_SMALL,BLOCK_HEIGHT_SMALL,color);
					}
			}
		}
}

void draw_speaker(uint8_t x, uint8_t y, uint16_t color) {
		uint8_t column = 0;
		uint8_t row = 0;
		uint8_t initial_x_coordinate = x;
		uint8_t initial_y_coordinate = y;
		for (row=0;row<17;row++) {
			for(column=0;column<13;column++) {
					if(speaker[row][column]==1) {
						LCD_DrawRectangle(initial_x_coordinate+column,initial_y_coordinate+row,1,1,color);
					}
			}
		}
}

void draw_sound(uint8_t x, uint8_t y, uint16_t color) {
		uint8_t column = 0;
		uint8_t row = 0;
		uint8_t initial_x_coordinate = x;
		uint8_t initial_y_coordinate = y;
		for (row=0;row<12;row++) {
			for(column=0;column<8;column++) {
					if(sound[row][column]==1) {
						LCD_DrawRectangle(initial_x_coordinate+column,initial_y_coordinate+row,1,1,color);
					}
			}
		}
}

void draw_cross(uint8_t x, uint8_t y, uint16_t color) {
		uint8_t column = 0;
		uint8_t row = 0;
		uint8_t initial_x_coordinate = x;
		uint8_t initial_y_coordinate = y;
		for (row=0;row<10;row++) {
			for(column=0;column<10;column++) {
					if(mute_cross[row][column]==1) {
						LCD_DrawRectangle(initial_x_coordinate+column,initial_y_coordinate+row,1,1,color);
					}
			}
		}
}

void shift_character_sx(uint8_t frame, uint8_t x_old, uint8_t y_old) {
		uint8_t i = 0,num_shift = 0,x_new,y_new;
		if (frame == 0) {
			while(num_shift<2) {
				x_new = x_old - (BLOCK_WIDTH*3);
				y_new = y_old;
				for (i=0;i<92;i+=2) {
					LCD_DrawRectangle(x_old+(first_clear_sx[i+1]*BLOCK_WIDTH),y_old+(first_clear_sx[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,White);
				}
				for (i=0;i<92;i+=2) {
					LCD_DrawRectangle(x_new+(first_fill_sx[i+1]*BLOCK_WIDTH),y_new+(first_fill_sx[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,Black);
				}
				x_old-=(BLOCK_WIDTH*3);
				num_shift++;
			}
		} else {
			while(num_shift<2) {
				x_new = x_old - (BLOCK_WIDTH*3);
				y_new = y_old;
				for (i=0;i<92;i+=2) {
					LCD_DrawRectangle(x_old+(second_clear_sx[i+1]*BLOCK_WIDTH),y_old+(second_clear_sx[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,White);
				}
				for (i=0;i<92;i+=2) {
					LCD_DrawRectangle(x_new+(second_fill_sx[i+1]*BLOCK_WIDTH),y_new+(second_fill_sx[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,Black);
				}
				x_old-=(BLOCK_WIDTH*3);
				num_shift++;
			}
		}
		character_x = x_new;
}

// Square to clear (row/column indexes related to old (before dx shift) position of character first frame to clear)
const uint8_t first_clear_dx[92] = {
		0,2,0,3,0,9,0,10,
		1,1,1,2,1,3,1,8,1,9,1,10,
		2,1,2,2,2,3,
		3,1,3,2,3,10,3,11,
		4,1,4,8,
		5,0,5,12,
		6,0,6,5,6,6,6,7,6,12,
		7,1,7,11,
		8,1,8,3,8,9,
		9,1,9,2,9,3,9,10,9,11,
		10,1,10,11,
		11,2,11,10,
		12,3,12,5,12,7,
		13,4,13,9,
		14,8
};

// Square to fill (row/column indexes related to new (after dx shift) position of character first frame to fill)
const uint8_t first_fill_dx[92] = {
		0,2,0,3,0,9,0,10,
		1,2,1,3,1,4,1,9,1,10,1,11,	
		2,9,2,10,2,11,
		3,1,3,2,3,10,3,11,
		4,4,4,11,
		5,0,5,12,
		6,0,6,5,6,6,6,7,6,12,
		7,1,7,11,
		8,1,8,3,8,12,
		9,1,9,2,9,3,9,10,9,11,
		10,1,10,11,
		11,2,11,10,
		12,5,12,7,12,9,
		13,7,13,9,
		14,8
};

// Square to clear (row/column indexes related to old (before dx shift) position of character second frame to clear)
const uint8_t second_clear_dx[92] = {
		0,2,0,3,0,9,0,10,
		1,1,1,2,1,3,1,8,1,9,1,10,
		2,1,2,2,2,3,
		3,1,3,2,3,10,3,11,
		4,1,4,8,
		5,0,5,12,
		6,0,6,5,6,6,6,7,6,12,
		7,1,7,11,
		8,0,8,9,8,11,
		9,1,9,2,9,9,9,10,9,11,
		10,1,10,11,
		11,2,11,10,
		12,3,12,5,12,7,
		13,3,13,5,
		14,4
};

// Square to fill (row/column indexes related to new (after dx shift) position of character second frame to fill)
const uint8_t second_fill_dx[92] = {
		0,2,0,3,0,9,0,10,
		1,2,1,3,1,4,1,9,1,10,1,11,
		2,9,2,10,2,11,
		3,1,3,2,3,10,3,11,
		4,4,4,11,
		5,0,5,12,
		6,0,6,5,6,6,6,7,6,12,
		7,1,7,11,
		8,3,8,9,8,11,
		9,1,9,2,9,9,9,10,9,11,
		10,1,10,11,
		11,2,11,10,
		12,5,12,7,12,9,
		13,3,13,8,
		14,4
};

void shift_character_dx(uint8_t frame, uint8_t x_old, uint8_t y_old) {
		uint8_t i = 0,num_shift = 0,x_new,y_new;
		if (frame == 0) {
			while(num_shift<2) {
				x_new = x_old + (BLOCK_WIDTH*3);
				y_new = y_old;
				for (i=0;i<92;i+=2) {
					LCD_DrawRectangle(x_old+(first_clear_dx[i+1]*BLOCK_WIDTH),y_old+(first_clear_dx[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,White);
				}
				for (i=0;i<92;i+=2) {
					LCD_DrawRectangle(x_new+(first_fill_dx[i+1]*BLOCK_WIDTH),y_new+(first_fill_dx[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,Black);
				}
				x_old+=(BLOCK_WIDTH*3);
				num_shift++;
			}
		} else {
			while(num_shift<2) {
				x_new = x_old + (BLOCK_WIDTH*3);
				y_new = y_old;
				for (i=0;i<92;i+=2) {
					LCD_DrawRectangle(x_old+(second_clear_dx[i+1]*BLOCK_WIDTH),y_old+(second_clear_dx[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,White);
				}
				for (i=0;i<92;i+=2) {
					LCD_DrawRectangle(x_new+(second_fill_dx[i+1]*BLOCK_WIDTH),y_new+(second_fill_dx[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,Black);
				}
				x_old+=(BLOCK_WIDTH*3);
				num_shift++;
			}
		}
		character_x = x_new;
}

void draw_battery_charge(uint8_t battery, uint8_t num_cells) {
		uint8_t i = 0, temp = 0;
		if (battery == 0) {
			temp = battery_cell_1;
			if (battery_cell_1+num_cells>5) {
				return;
			}
			battery_cell_1+=num_cells;
			for (i=temp;i<battery_cell_1;i++) {
				LCD_DrawRectangle(POS_FIRST_BATTERY+OFFSET+(i*9),48,7,19,Black);
			}
		} else {
			temp = battery_cell_2;
			if (battery_cell_2+num_cells>5) {
				return;
			}
			battery_cell_2+=num_cells;
			for (i=temp;i<battery_cell_2;i++) {
				LCD_DrawRectangle(POS_SECOND_BATTERY+OFFSET+(i*9),48,7,19,Black);
			}
		}
}

void draw_battery_discharge(uint8_t battery, uint8_t num_cells) {
		uint8_t i = 0, temp = 0;
		if (battery == 0) {
			temp = battery_cell_1;
			if (battery_cell_1 - num_cells<0) {
				return;
			}
			battery_cell_1-=num_cells;
			for (i=temp;i>battery_cell_1;i--) {
				LCD_DrawRectangle(POS_FIRST_BATTERY+OFFSET+((i-1)*(WIDTH_CELL+2)),POS_BATTERY_Y+8,WIDTH_CELL,HEIGHT_CELL,White);
			}
		} else {
			temp = battery_cell_2;
			if (battery_cell_2 - num_cells<0) {
				return;
			}
			battery_cell_2-=num_cells;
			for (i=temp;i>battery_cell_2;i--) {
				LCD_DrawRectangle(POS_SECOND_BATTERY+OFFSET+((i-1)*(WIDTH_CELL+2)),POS_BATTERY_Y+8,WIDTH_CELL,HEIGHT_CELL,White);
			}
		}
}

void draw_character(uint8_t x, uint8_t y, uint16_t color) {
		uint8_t column = 0;
		uint8_t row = 0;
		uint8_t initial_x_coordinate = x;
		uint8_t initial_y_coordinate = y;
		for (row=0;row<15;row++) {
			for(column=0;column<13;column++) {
					if(first_frame_character[row][column]==1) {
						LCD_DrawRectangle(initial_x_coordinate+(column*BLOCK_WIDTH),initial_y_coordinate+(row*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,color);
					}
			}
		}
}

void draw_candy(uint8_t x, uint8_t y, uint16_t color) {
	uint8_t size = 30;
	LCD_DrawRectangle(x,y,size,12,color);
	LCD_DrawRectangle(x-9,y+3,9,6,color);
	LCD_DrawRectangle(x+size,y+3,9,6,color);
}

void draw_burger(uint8_t x, uint8_t y, uint16_t color) {
	uint8_t size = 40;
	LCD_DrawRectangle(x,y,size,12,color);
	LCD_DrawRectangle(x+5,y-8,30,8,color);
	LCD_DrawRectangle(x+5,y+12,30,8,color);
}

void change_character(uint8_t direction,uint8_t x, uint8_t y) {
	uint8_t i = 0;
	if (direction == 0) {
		for(i=0;i<14;i+=2) {
			LCD_DrawRectangle(x+(clear[i+1]*BLOCK_WIDTH),y+(clear[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,White);
		}
		for(i=0;i<14;i+=2) {
			LCD_DrawRectangle(x+(fill[i+1]*BLOCK_WIDTH),y+(fill[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,Black);
		}
	} else {
		for(i=0;i<14;i+=2) {
			LCD_DrawRectangle(x+(fill[i+1]*BLOCK_WIDTH),y+(fill[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,White);
		}
		for(i=0;i<14;i+=2) {
			LCD_DrawRectangle(x+(clear[i+1]*BLOCK_WIDTH),y+(clear[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,Black);
		}
	}
}

void clear_changing(uint8_t x, uint8_t y) {
	uint8_t i;
	if (frame == 1) {
		for(i=0;i<14;i+=2) {
			LCD_DrawRectangle(x+(fill[i+1]*BLOCK_WIDTH),y+(fill[i]*BLOCK_HEIGHT),BLOCK_WIDTH,BLOCK_HEIGHT,White);
		}
	}
}

void draw_structure_and_border(void) {
	GUI_Text(70, 1, (uint8_t *) "AGE: 00:00:00", Black, White);
	GUI_Text(20, 20, (uint8_t *) "Happiness", Black, White);
	GUI_Text(150, 20, (uint8_t *) "Satiety", Black, White);
	GUI_Text(40, 275, (uint8_t *) "Meal", Black, White);
	GUI_Text(160, 275, (uint8_t *) "Snack", Black, White);
	LCD_DrawLine(0,0,240,0,Black); //Sopra
	LCD_DrawLine(0,319,240,319,Black); //Sotto
	LCD_DrawLine(0,0,0,320,Black); //Sinistra
	LCD_DrawLine(239,0,239,320,Black); //Destra
	LCD_DrawLine(0,250,240,250,Black); //linea sotto orizzontale
	LCD_DrawLine(120,250,120,320,Black); // linea centrale verticale
	LCD_DrawRectangle(POS_FIRST_BATTERY,POS_BATTERY_Y,WIDTH_BATTERY,HEIGHT_BATTERY,Black); // Prima Batteria
	LCD_DrawRectangle(POS_FIRST_BATTERY+5,POS_BATTERY_Y+5,WIDTH_BATTERY-10,HEIGHT_BATTERY-10,White);
	LCD_DrawRectangle(POS_FIRST_BATTERY+WIDTH_BATTERY+2,POS_BATTERY_Y+10,5,15,Black);
	LCD_DrawRectangle(POS_SECOND_BATTERY,POS_BATTERY_Y,WIDTH_BATTERY,HEIGHT_BATTERY,Black);	// Seconda Batteria
	LCD_DrawRectangle(POS_SECOND_BATTERY+5,POS_BATTERY_Y+5,WIDTH_BATTERY-10,HEIGHT_BATTERY-10,White);
	LCD_DrawRectangle(POS_SECOND_BATTERY+WIDTH_BATTERY+2,POS_BATTERY_Y+10,5,15,Black);
	draw_battery_charge(0,5);
	draw_battery_charge(1,5);
	draw_speaker(10,2,Black);
}

void draw_options(int option, uint16_t color) {
	LCD_DrawLine(120,250,120,320,color); // linea centrale verticale
	if(option ==1) {
		LCD_DrawLine(0,250,0,320,color); // linea verticale a sx
		LCD_DrawLine(0,250,120,250,color); //linea sotto orizzontale
		LCD_DrawLine(0,319,120,319,color); //Sotto
	} else {
		LCD_DrawLine(239,250,239,320,color); // linea verticale a dx
		LCD_DrawLine(120,250,240,250,color); //linea sotto orizzontale
		LCD_DrawLine(120,319,240,319,color); //Sotto
	}
}

void update_age(uint8_t hours,uint8_t minutes, uint8_t seconds) {
	char text[9];
	//text[8] = '\0';
	sprintf(text,"%02d:%02d:%02d",hours,minutes,seconds);
	GUI_Text(110, 1, (uint8_t *) text, Black, White);
}

void reset_pressed(void) {
	LCD_DrawCircle(120,160,80,White);
	GUI_Text(60, 145, (uint8_t *) "your tamagotchi", White, White);
	GUI_Text(85, 165, (uint8_t *) "ran away", White, White);
	GUI_Text(100, 275, (uint8_t *) "Reset", White, White);
  battery_cell_1 = 0;
	battery_cell_2 = 0;
  character_x = 75;
	character_y = 130;
  frame = 0;
  is_shifting = 0;
  is_flipping = 0;
  joystick_selection = 0;
  selection_disabled = 0;
  reset = 0;
  time_elapsed = 0;
	draw_structure_and_border();
	draw_character(character_x,character_y,Black);
	reset_timer(0);
	reset_RIT();
	enable_timer(0);
	enable_timer(2);
}

void eat_burger(void) {
	is_shifting = 1;
	shift_character_sx(frame,character_x,character_y);
	is_shifting = 0;
	draw_burger(POS_BURGER_X,POS_BURGER_Y,Black);
	is_animating = 1;
}

void eat_candy(void) {
	is_shifting = 1;
	shift_character_dx(frame,character_x,character_y);
	is_shifting = 0;
	draw_candy(POS_CANDY_X,POS_CANDY_Y,Black);
	is_animating = 1;
}
