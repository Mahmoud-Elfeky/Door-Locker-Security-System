/******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.h
 *
 * Description: Header file for the TIMER driver
 *
 * Author: Mahmoud Elfeky
 *
 *******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
typedef enum {
	TIMER_ZERO,TIMER_ONE,TIMER_TWO
}TimerNumber;
typedef enum {
	OVERFLOW,CTC
}TimerMode;

typedef enum {
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,XTAL_CLOCK_ON_FALLING,XTAL_CLOCK_ON_RISING
}TimerClock;

typedef struct {
	TimerNumber e_number;//zero, one or two
	TimerMode e_mode;//overflow or ctc
	TimerClock e_clock;
	uint16 initValue;
	uint16 compareValue;
}TimerConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Initialize the TIMER Module that can be t0, t1 or t2:
 * 1. Setup the LCD pins directions by use the GPIO driver.
 * 2. Setup the LCD Data Mode 4-bits or 8-bits.
 */
void TIMER_init(TimerConfigType* timerConfigs);

void TIMER_setCallback(void (*a_ptr)(void),TimerNumber timer_no);

void TIMER_deInit(TimerNumber e_timerNumber);
#endif /* TIMER_H_ */
