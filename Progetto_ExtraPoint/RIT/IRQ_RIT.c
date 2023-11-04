/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../Character/character.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "../adc/adc.h"
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void RIT_IRQHandler (void)
{					
	static int click = 0;
	ADC_start_conversion();
	click++;
	if (joystick_disabled == 0) {
		if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
			/* Joystick select pressed */
			switch(click){
				case 1:
					if (reset == 1) {
						reset_pressed();
					}
					switch(joystick_selection) {
						case 1:
							click_note();
							selection_disabled = 1;
							if (is_flipping == 0) {
								eat_burger();
							} else {
								select_pressed = 1;
							}
							break;
						case 2:
							click_note();
							selection_disabled = 1;
							if (is_flipping == 0) {
								eat_candy();
							} else {
								select_pressed = 1;
							}
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
		else if ((LPC_GPIO1->FIOPIN & (1<<26)) == 0) {
			/* Joystick down pressed */
			switch(click){
				case 1:
					break;
				default:
					break;
			}
		}
		else if ((LPC_GPIO1->FIOPIN & (1<<27)) == 0) {
			/* Joystick left pressed */
			switch(click){
				case 1:
						if (selection_disabled == 0) {
							joystick_selection = 1;
							draw_options(2,Black);
							draw_options(joystick_selection,Red);
						}			
					break;
				default:
					break;
			}
		}
		else if ((LPC_GPIO1->FIOPIN & (1<<28)) == 0) {
			/* Joystick right pressed */
			switch(click){
				case 1:
						if (selection_disabled == 0) {
							joystick_selection = 2;
							draw_options(1,Black);
							draw_options(joystick_selection,Red);
						}
					break;
				default:
					break;
			}
		} 
		else if ((LPC_GPIO1->FIOPIN & (1<<29)) == 0) {
			/* Joystick up pressed */
			switch(click){
				case 1:
					break;
				default:
					break;
			}
		} else { /*Joystick Released*/
			click = 0;
		}
	}
	
	LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
