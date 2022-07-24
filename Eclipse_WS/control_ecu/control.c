 /******************************************************************************
 *
 * Module: CONTROLLER MC
 *
 * File Name: control.c
 *
 * Description: Source file for the CONTROLLER MC
 *
 * Author: Mahmoud Elfeky
 *
 *******************************************************************************/

#include<avr/io.h>
#include "twi.h"
#include "uart.h"
#include "timer.h"
#include "buzzer.h"
#include "dc_motor.h"
#include "external_eeprom.h"
#include <avr/interrupt.h>

/*these defines used as encoding between two micros as a protocol of communication*/
#define OPEND_DOOR '+'
#define PLAY_BUZZER '&'
#define SET_PASSWORD '#'
#define VALIDATE_PASSWORD '?'

uint16 tick=0,finalTick=0;

/*this function for getting the password from MC1*/
void get_password();
/*this function for asking the MC1 to open the door that is connected to it */
void opentTheDoor();
/*this function for asking the MC1 to play the buzzer that is connected to it as an alarm*/
void playBuzzer();
/*this function for asking the MC1 to validate the password that is entered by user*/
void validatePassword();
/*this function for performing delay process in milisec */
void delay(uint16 period);
/*this function for acting as a call back function that is called by ISR after interrupt occuring*/
void startDelay();
/*these 2 array is used for saving passwords as string that user input them at first use*/
uint8 password1[6] , password2[6];

void main(void){
	sei();	/*enable global interrupt*/
	//Initialize buzzer and motor that is connected to the door
	BUZZER_init();
	DcMotor_Init();

	/*Initialization code*/

	/*initialize the uart module with dynamic configuration
	 * character size 8 bit
	 * parityMode disabled
	 * stopBitSelect is only one bit
	 * baud rate is 9600
	 * */
	UartConfigType s_uartSet={EIGHT_BIT,DISABLED,ONE_BIT,9600};
	UART_init(&s_uartSet);

	/*initialize the twi module with dynamic configuration
	 *MC twi device address which is 0x01
	 * bit rate is 400.000 kpbs
	 */
	TWIConfigType s_i2cSet={0x01,_400KBPS};
	TWI_init(&s_i2cSet);

	/*choice to hold the choice that user need/inputed to operate based on it  */
	uint8 choice;
	while(1){

		/*Application code*/

		//receive the choice from mc1
		choice=UART_recieveByte();
		switch(choice){
			case OPEND_DOOR :
				opentTheDoor();
				break;
			case PLAY_BUZZER :
				playBuzzer();
				break;
			case SET_PASSWORD :
				get_password();
				break;
			case VALIDATE_PASSWORD:
				validatePassword();
				break;
		}
	}
}
void get_password(){
	//get the password from the MC1
	do{
		/*get first try from MC1 and store it in array*/
		UART_receiveString(password1);
		/*get second try from MC1  and store it in array*/
		UART_receiveString(password2);
		/*check if 2 tries are same or not*/
		if(strcmp(password1,password2) == 0){
			/* Write password in the external EEPROM */
			uint8 index=0;
			for( ; index<5 ; index++)
			{
				EEPROM_writeByte(0x0311+index,password1[index]);
				delay(10);
			}
			password1[index]='\0';

			/*if 2 tries are the same send '1' to MC1 as agreement of correct*/
			UART_sendByte('1');
			break;
		}
		else
			/*if 2 tries are not the same send '0' to MC1 as agreement of not correct*/
			UART_sendByte('0');
	}while(1);/*repeat while input is not matched*/

}
/*
 * open buzzer for 1 minute as an alarm then turn it off*/
void playBuzzer(){
	BUZZER_on();
	delay(60000);
	BUZZER_off();
}
/*open the door for 15 sec then wait 3 sec then close it in 15 sec*/
void opentTheDoor(){
	DcMotor_Rotate(CW);
	delay(15000);
	DcMotor_Rotate(STOP);
	delay(3000);
	DcMotor_Rotate(ACW);
	delay(15000);
	DcMotor_Rotate(STOP);
}
/*this function for validating whether password the user inputed
 * match the one that was saved or not and inform the MC1 with result*/
void validatePassword(){
	UART_receiveString(password1);

	/* Read password from the external EEPROM */
	for(uint8 index=0 ; index<5 ; index++)
	{
		EEPROM_readByte(0x0311+index,password2+index);
		delay(10);
	}

	if(strcmp(password1,password2) == 0){
		UART_sendByte('1');
	}
	else{
		UART_sendByte('0');
	}
}
/*intialize number of ticks and final ticks
 * configure timer module to play timer1 in ctc mode with prescaler = FCPU/256
 * and compare vale 3125 which is enough to calculate 100ms
 * set the call back function which update ticks based on interrupts
 * wait until tick is finished which achieve delay*/
void delay(uint16 period){
	tick=0;
	finalTick=period/100;
	TimerConfigType s_timerSet={TIMER_ONE,CTC,F_CPU_256,0,3125};
	TIMER_init(&s_timerSet);
	TIMER_setCallback(startDelay,TIMER_ONE);
	while(tick!=finalTick);

}
/* used as call back function which update ticks based
 * on interrupts and this calling performed by ISR*/
void startDelay(){
	tick++;
	if(tick==finalTick){
		TIMER_deInit(TIMER_ONE);
	}
}

