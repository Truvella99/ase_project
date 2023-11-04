/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../timer/timer.h"
#include "../character/character.h"
#include "../GLCD/GLCD.h"
/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

/* k=1/f'*f/n  k=f/(f'*n) k=25MHz/(f'*45) */

void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
  if(AD_current <= AD_last - 20 || AD_current >= AD_last + 20){
		// Volume values into a range of 1024
		volume = AD_current/4;
		AD_last = AD_current;
  }
	// handle the mute/sound
	if (volume == 0) {
		draw_sound(25,5,White);
	  draw_cross(25,6,Black);
	} else {
		draw_cross(25,6,White);
		draw_sound(25,5,Black);
	}
	
}
