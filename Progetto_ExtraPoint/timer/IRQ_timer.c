/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../Character/character.h"
#include "../TouchPanel/TouchPanel.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
static uint8_t animation_time = 0;
// 1 click, 2 eat, 3 death 4 cuddle
static uint8_t sound = 0;

void TIMER0_IRQHandler (void)
{
	uint8_t hours = 0,minutes = 0,seconds = 0;
	// elapsed time
	time_elapsed++;
	if (is_animating == 1) {
		animation_time++;
		if (animation_time == 2 && sound == 0) {
			eat_note();
		}
	}
	hours = time_elapsed/3600;
	minutes = (time_elapsed-(hours * 3600))/60;
	seconds = time_elapsed - ((hours * 3600)+(minutes * 60));
	update_age(hours,minutes,seconds);
	if (time_elapsed%5==0 && time_elapsed!=0) {
		draw_battery_discharge(0,1);
		draw_battery_discharge(1,1);
	}
	// character flip
	if (is_shifting == 0) {
		is_flipping = 1;
		if (frame == 0) {
			change_character(frame,character_x,character_y);
			frame = 1;
		} else {
			change_character(frame,character_x,character_y);
			frame = 0;
		}
		is_flipping = 0;
	}
	// reset 
	if (battery_cell_1 == 0 || battery_cell_2 == 0) {
		death_note();
		disable_timer(0);
		disable_timer(2);
		joystick_disabled = 0;
		draw_heart(110,120,White);
		draw_heart(50,170,White);
		draw_heart(171, 170,White);
		draw_burger(POS_BURGER_X,POS_BURGER_Y,White);
		draw_candy(POS_CANDY_X,POS_CANDY_Y,White);
		GUI_Text(50, 110, (uint8_t *) "Bye!!", Black, White);
		draw_character(character_x,character_y,White);
		clear_changing(character_x,character_y);
		GUI_Text(50, 110, (uint8_t *) "Bye!!", White, White);
		GUI_Text(40, 275, (uint8_t *) "Meal", White, White);
		GUI_Text(160, 275, (uint8_t *) "Snack", White, White);
		LCD_DrawCircle(120,160,80,Black);
		GUI_Text(60, 145, (uint8_t *) "your tamagotchi", Black, White);
		GUI_Text(85, 165, (uint8_t *) "ran away", Black, White);
		LCD_DrawLine(120,250,120,320,White); // linea centrale verticale
		GUI_Text(100, 275, (uint8_t *) "Reset", Black, White);
		LCD_DrawLine(0,250,240,250,Red); //linea sotto orizzontale
		LCD_DrawLine(0,319,240,319,Red); //Sotto
		LCD_DrawLine(0,250,0,320,Red); // linea verticale a sx
		LCD_DrawLine(239,250,239,320,Red); // linea verticale a dx
		reset = 1;
	}
	// if select pressed while flipping
	if (select_pressed == 1) {
		switch(joystick_selection) {
					case 1:
							eat_burger();
						break;
					case 2:
							eat_candy();
						break;
					default:
						break;
		}
		select_pressed = 0;
	}
	
	if (animation_time == 2) {
		if (joystick_disabled == 1) {
			// Cuddle Animation End
			draw_heart(110,120,White);
			draw_heart(50,170,White);
			draw_heart(171, 170,White);
			draw_battery_charge(0,1);
		}
	}
	
	//animations
	if (animation_time == 3) {
			switch(joystick_selection) {
				case 1:
					if (is_flipping == 0) {
						draw_burger(POS_BURGER_X,POS_BURGER_Y,White);
						is_shifting = 1;
						shift_character_dx(frame,character_x,character_y);
						is_shifting = 0;
						draw_battery_charge(1,1);
						draw_options(joystick_selection,Black);
					}
					selection_disabled = 0;
					break;
				case 2:
					if (is_flipping == 0) {
						draw_candy(POS_CANDY_X,POS_CANDY_Y,White);
						is_shifting = 1;
						shift_character_sx(frame,character_x,character_y);
						is_shifting = 0;
						draw_battery_charge(0,1);
						draw_options(joystick_selection,Black);
					}
					selection_disabled = 0;
					break;
				default:
					selection_disabled = 0;
					break;
			}
		NVIC_EnableIRQ(TIMER2_IRQn);
		joystick_disabled = 0;
		joystick_selection = 0;
		is_animating = 0;
		animation_time = 0;
	}
	
	LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
} 


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
const int freqs[8]={2120,1890,1684,1592,1417,1263,1125,1062};

void set_note (uint8_t note) {
	disable_timer(3);
	reset_timer(3);
	init_timer(3,freqs[note]);
	enable_timer(3);
}

void click_note (void) {
	sound = 1;
	enable_timer(3);
	enable_timer(1);
	set_note(0);
}

void eat_note(void) {
	sound = 2;
	enable_timer(3);
	enable_timer(1);
	set_note(0);
}

void death_note(void) {
	sound = 3;
	enable_timer(3);
	enable_timer(1);
	set_note(0);
}

void cuddle_note(void) {
	sound = 4;
	enable_timer(3);
	enable_timer(1);
	set_note(0);
}

void TIMER1_IRQHandler (void)
{
	static uint8_t click_notes = 0;
	static uint8_t eat_notes = 0;
	static uint8_t death_notes = 0;
	static uint8_t cuddle_notes = 0;
	
	switch(sound) {
		case 1:
			click_notes++;
			if (click_notes == 1) {
				disable_timer(3);
				disable_timer(1);
				click_notes = 0;
				sound = 0;
			} else {
				set_note(click_notes);
			}
			break;
		case 2:
			eat_notes++;
			if (eat_notes == 3) {
				disable_timer(3);
				disable_timer(1);
				eat_notes = 0;
				sound = 0;
			} else {
				set_note(eat_notes);
			}
			break;
		case 3:
			death_notes++;
			if(death_notes == 5) {
				disable_timer(3);
				disable_timer(1);
				death_notes = 0;
				sound = 0;
			} else {
				set_note(death_notes);
			}
			break;
		case 4:
			cuddle_notes++;
			if(cuddle_notes == 8) {
				disable_timer(3);
				disable_timer(1);
				cuddle_notes = 0;
				sound = 0;
			} else {
				set_note(cuddle_notes);
			}
			break;
		default:
			break;
	}
	
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER2_IRQHandler (void)
{
	if (getDisplayPoint(&display, Read_Ads7846(), &matrix)) {
		if ((character_x == 75 && character_y == 130) && (display.x >= character_x - 21 && display.x <= character_x + 112) && (display.y >= character_y - 14 && display.y <= character_y + 119)) {
				// Cuddle Animation
				NVIC_DisableIRQ(TIMER2_IRQn);
				cuddle_note();
				joystick_disabled = 1;
				if (joystick_selection != 0) {
					draw_options(joystick_selection,Black);
					joystick_selection = 0;
				}
				is_animating = 1;
				draw_heart(110,120,Red);
				draw_heart(50,170,Red);
				draw_heart(171, 170,Red);
		}
	}
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
** Function name:		Timer3_IRQHandler
**
** Descriptions:		Timer/Counter 3 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint16_t SinTable[45] =                                      
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

void TIMER3_IRQHandler (void)
{
	static int ticks = 0;
	LPC_DAC->DACR = (((SinTable[ticks])*volume)/819)<<6;
	ticks++;
	
	if (ticks==45) {
		ticks=0;
	}
	
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
