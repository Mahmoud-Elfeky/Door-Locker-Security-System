/*
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the BUZZER device
 *
 * Author: Mahmoud Elfeky
 */
#include"gpio.h"
#include "buzzer.h"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the buzzer device by:
 * 1. Setup the direction of its pin and Initialize it as off
 */
void BUZZER_init(void){
GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID,PIN_OUTPUT);
GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}
/*
 * Description :
 * Functional responsible for turn on the buzzer device by:
 * 1. Setup the pin to out 5v on it's pin
 */
void BUZZER_on(void){
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_HIGH);
}
/*
 * Description :
 * Functional responsible for turn off the buzzer device by:
 * 1. Setup the pin to out 0v on it's pin
 */
void BUZZER_off(void){
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}

