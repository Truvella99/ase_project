/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "Character/character.h"
#include "RIT/RIT.h"
#include "TouchPanel/TouchPanel.h"
#include "adc/adc.h"
#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

void joystick_init(void) {
	/* joystick select functionality */
  LPC_PINCON->PINSEL3 &= ~(3<<26);	//PIN mode GPIO (00b value per P1.29)
	LPC_GPIO1->FIODIR   &= ~(1<<29);	//P1.25 Input (joysticks on PORT1 defined as Input) 
}

void DAC_init(void) {
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
}

int main(void)
{
	SystemInit();  												/* System Initialization (i.e., PLL),touch panel, joystick and gui  */
  LCD_Initialization();
	TP_Init();
	TouchPanel_Calibrate();
	LCD_Clear(White);
	joystick_init();
	ADC_init();
	DAC_init();
	LPC_SC->PCONP |= 1 << 22; //(Timer 2 Enable)
	LPC_SC->PCONP |= 1 << 23; //(Timer 3 Enable)
	draw_structure_and_border();
	draw_character(character_x,character_y,Black);
	// RIT to poll the joystick
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
	enable_RIT();
	// timer0 to count 1 second */
	init_timer(0, 0x17D7840 ); 						/* 1s * 25MHz = 25*10^6 = 0x17D7840 */
	enable_timer(0);
	// timer 1 to handle the duration of notes (0.25 second)
	init_timer(1, 0x5F5E10 ); 						/* 0.25s * 25MHz = 25*10^6/4 = 0x5F5E10 */
	// timer 2 to poll the value of touchpanel (8us * 25 MHz = 200 = 0xC8)
	init_timer(2, 0xC8);
	enable_timer(2);
	// timer 3 to handle the note creation (initilialized to note 0)
	init_timer(3,2120);
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
