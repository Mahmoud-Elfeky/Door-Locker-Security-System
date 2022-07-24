 /******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.c
 *
 * Description: Source file for the AVR TIMER driver
 *
 * Author: Mahmoud Elfeky
 *
 *******************************************************************************/

#include "timer.h"
#include "common_macros.h" 	/* To use the macros like SET_BIT */
#include <avr/io.h> 		/* To use Timers Registers */
#include <avr/interrupt.h> 	/* For timers ISR */


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */

 static volatile void (*g_callBackPtrT0)(void) = NULL_PTR;
 static volatile void (*g_callBackPtrT1)(void) = NULL_PTR;
 static volatile void (*g_callBackPtrT2)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect){
	if(g_callBackPtrT0!=NULL_PTR)
		(*g_callBackPtrT0)();
}

ISR(TIMER0_COMP_vect){
	if(g_callBackPtrT0!=NULL_PTR)
		(*g_callBackPtrT0)();
}
ISR(TIMER1_OVF_vect){
	if(g_callBackPtrT1!=NULL_PTR)
		(*g_callBackPtrT1)();
}

ISR(TIMER1_COMPA_vect){
	if(g_callBackPtrT1!=NULL_PTR)
		(*g_callBackPtrT1)();
}
ISR(TIMER2_OVF_vect){
	if(g_callBackPtrT2!=NULL_PTR)
		(*g_callBackPtrT2)();
}

ISR(TIMER2_COMP_vect){
	if(g_callBackPtrT2!=NULL_PTR)
		(*g_callBackPtrT2)();
}
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the ICU driver
 * 	1. Set the required clock.
 * 	2. Set the required edge detection.
 * 	3. Enable the Input Capture Interrupt.
 * 	4. Initialize Timer1 Registers
 */

void TIMER_init(TimerConfigType* timerConfigs){
	/*this for timer0 (overflow or CTC)*/
	if(timerConfigs->e_number==TIMER_ZERO){
		if(timerConfigs->e_mode==OVERFLOW){
			TCCR0|=(1<<FOC0);//determine Non-PWM mode and overflow mode and OC is off
			TCCR0|=((TCCR0 & 0xF8)|(timerConfigs->e_clock&0x07)); //set the clock
			TCNT0=timerConfigs->initValue;
			TIMSK|=(1<<TOIE0);//enable interrupt for overflow module
			//flag is cleared automatically when execute ISR
		}
		else if(timerConfigs->e_mode==CTC){
			TCCR0|=(1<<FOC0)|(1<<WGM01);//determine Non-PWM mode and CTC mode and OC is off
			TCCR0|=((TCCR0 & 0xF8)|(timerConfigs->e_clock&0x07)); //set the clock
			TCNT0=timerConfigs->initValue;
			OCR0=timerConfigs->compareValue;
			TIMSK|=(1<<OCIE0);//enable interrupt for CTC module
			//flag is cleared automatically when execute ISR
		}
	}
	/*this for timer1 (overflow or CTC)*/
	else if(timerConfigs->e_number==TIMER_ONE){
		/*this for timer1 (overflow or CTC)*/
		if(timerConfigs->e_mode==OVERFLOW){
			TCCR1A|=(1<<FOC1A);//determine Non-PWM mode and overflow mode and OC is off
			TCCR1B|=(TCCR1B & 0xF8)|((timerConfigs->e_clock) &0x07); //set the clock
			TCNT1=timerConfigs->initValue;
			TIMSK|=(1<<TOIE1);//enable interrupt for overflow module
			//flag is cleared automatically when execute ISR
		}
		else if(timerConfigs->e_mode==CTC){
			TCCR1A|=(1<<FOC1A);//determine Non-PWM mode and OC is off
			TCCR1B|=(1<<WGM12);//determine CTC mode
			TCCR1B|=(TCCR1B & 0xF8)|((timerConfigs->e_clock) &0x07); //set the clock
			TCNT1=timerConfigs->initValue;
			OCR1A=timerConfigs->compareValue;
			TIMSK|=(1<<OCIE1A);//enable interrupt for CTC module
			//flag is cleared automatically when execute ISR
		}
	}

	/*this for timer2 (overflow or CTC)*/
	else if(timerConfigs->e_number==TIMER_TWO){
		if(timerConfigs->e_mode==OVERFLOW){
			TCCR2|=(1<<FOC2);//determine Non-PWM mode and overflow mode and OC is off
			TCCR2|=((TCCR2 & 0xF8)|(timerConfigs->e_clock&0x07)); //set the clock
			TCNT2=timerConfigs->initValue;
			TIMSK|=(1<<TOIE2);//enable interrupt for overflow module
			//flag is cleared automatically when execute ISR
		}
		else if(timerConfigs->e_mode==CTC){
			TCCR2|=(1<<FOC2)|(1<<WGM21);//determine Non-PWM mode and CTC mode and OC is off
			TCCR2|=((TCCR2 & 0xF8)|(timerConfigs->e_clock&0x07)); //set the clock
			TCNT2=timerConfigs->initValue;
			OCR2=timerConfigs->compareValue;
			TIMSK|=(1<<OCIE2);//enable interrupt for CTC module
			//flag is cleared automatically when execute ISR
		}
	}
}
//(1/((1/(8000000/pre_clock))*256))*15 //calculate cnt to delay for 15s with any clock
/*
 * Description: Function to set the Call Back function address.
 */
void TIMER_setCallback(void (*a_callBackPtr)(void),TimerNumber timer_no){
	if(timer_no==TIMER_ZERO)
		g_callBackPtrT0 = a_callBackPtr;
	else if(timer_no==TIMER_ONE)
		g_callBackPtrT1 = a_callBackPtr;
	else if(timer_no==TIMER_TWO)
		g_callBackPtrT2 = a_callBackPtr;
}

void TIMER_deInit(TimerNumber e_timerNumber){
	if(e_timerNumber == TIMER_ZERO){
		TCCR0=0;//stop clock and give all bits 0 value
		TCNT0=0;
		OCR0=0;
		TIMSK&=~(1<<TOIE0);//disable interrupt for overflow module
		TIMSK&=~(1<<OCIE0);//disable interrupt for CTC module

		TIFR|=(1<<TOV0);//clear overflow flag for t0
		TIFR|=(1<<OCF0);//clear ctc flag for t0
	}
	else if(e_timerNumber == TIMER_ONE){
		TCCR1A=0;
		TCCR1B= 0; //stop the clock
		TCNT1=0;
		OCR1A=0;

		TIMSK&=~(1<<TOIE1);//disable interrupt for overflow module
		TIMSK&=~(1<<OCIE1A);//disable interrupt for CTC module

		TIFR|=(1<<TOV1);//clear overflow flag for t1
		TIFR|=(1<<OCF1A);//clear ctc flag for t1
	}
	else if(e_timerNumber == TIMER_TWO){
		TCCR2=0;//stop clock and give all bits 0 value
		TCNT2=0;
		OCR2=0;
		TIMSK&=~(1<<TOIE2);//disable interrupt for overflow module
		TIMSK&=~(1<<OCIE2);//disable interrupt for CTC module

		TIFR|=(1<<TOV2);//clear overflow flag for t2
		TIFR|=(1<<OCF2);//clear ctc flag for t2
	}
}

