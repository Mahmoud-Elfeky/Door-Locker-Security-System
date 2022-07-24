 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the BUZZER device
 *
 * Author: Mahmoud Elfeky
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* BUZZER HW Ports and Pins Ids */
#define BUZZER_PORT_ID                  PORTD_ID
#define BUZZER_PIN_ID                   PIN2_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Initialize the BUZZER:
 * 1. Setup the BUZZER pins directions by use the GPIO driver.
 */
void BUZZER_init(void);
/*
 * Description :
 * Functional responsible for turn on the buzzer device by:
 * 1. Setup the pin to out 5v on it's pin
 */
void BUZZER_on(void);
/*
 * Description :
 * Functional responsible for turn off the buzzer device by:
 * 1. Setup the pin to out 0v on it's pin
 */
void BUZZER_off(void);

#endif /* BUZZER_H_ */
