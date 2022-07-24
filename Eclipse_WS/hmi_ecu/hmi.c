/*
 * hmi.c
 *
 *  Created on: Jun 23, 2022
 *      Author: EL-FEKY
 */
#include<avr/io.h>
#include "lcd.h"
#include"uart.h"
#include"timer.h"
#include "keypad.h"
#include<avr/interrupt.h>
/*these defines used as encoding between two micros as a protocol of communication*/
#define OPEND_DOOR '+'
#define PLAY_BUZZER '&'
#define CHANGE_PASSWORD '-'
#define SET_PASSWORD '#'
#define VALIDATE_PASSWORD '?'

/*these 2 array is used for saving passwords as string that user input them at first use*/
uint8 recieved_data[20],password[8];
uint16 tick=0,finalTick=0;
/*this flag is set only when the password is set for the first time*/
uint8 set_pass_flag=0;

/*this function for setting the password for the first time MC1*/
void set_password();
void home_screen();
uint8 get_and_check_password();
/*this function for performing delay process in milisec */
void delay(uint16 period);
/*this function for acting as a call back function that is called by ISR after interrupt occuring*/
void startDelay();

void main(void){

	sei();/*enable global interrupt*/

	/*initialize the lcd module with static configuration*/
	LCD_init();

	/*initialize the uart module with dynamic configuration
	 * character size 8 bit
	 * parityMode disabled
	 * stopBitSelect is only one bit
	 * baud rate is 9600
	 * */

	UartConfigType s_uartSet={EIGHT_BIT,DISABLED,ONE_BIT,9600};
	UART_init(&s_uartSet);

	while(1){
		/*Application code*/

		//set password for first use
		if(!set_pass_flag){
			UART_sendByte(SET_PASSWORD);
			set_password();
		}
		home_screen();
	}
}

void set_password(){
	uint8 key,result;
	do{
		LCD_clearScreen();
		LCD_moveCursor(0,0);
		LCD_displayString("Enter a password");
		for(uint8 num=0;num<6;num++){
			key=KEYPAD_getPressedKey();
			if(key!=13 && num!=5){
				password[num]=key+'0';
				LCD_moveCursor(1,num);
				LCD_displayCharacter('*');
			}
			else if (key==13 && num==5){
				password[num]='#';
				password[num+1]='\0';
				UART_sendString(password);
				LCD_clearScreen();
			}
			else
				--num;
			delay(200);
		}
		LCD_moveCursor(0,0);
		LCD_displayString("Reenter password");

		for(uint8 num=0;num<6;num++){
			key=KEYPAD_getPressedKey();
			if(key!=13 && num!=5){
				password[num]=key+'0';
				LCD_moveCursor(1,num);
				LCD_displayCharacter('*');
			}
			else if (key==13 && num==5){
				password[num]='#';
				password[num+1]='\0';
				UART_sendString(password);
				LCD_clearScreen();
			}
			else
				--num;
			delay(200);
		}
		result=UART_recieveByte();
		if(result=='0'){
			LCD_displayString("Not Correct!");
			delay(1000);
		}
	}while(result != '1');
	set_pass_flag=1;
	LCD_displayString("Correct");
	delay(1000);
	LCD_clearScreen();
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

void home_screen(){
	uint8 choice;
	do{
	LCD_displayString("+open the door");
	LCD_moveCursor(1,0);
	LCD_displayString("-change password");
	choice=KEYPAD_getPressedKey();
	delay(200);
	}while((choice != CHANGE_PASSWORD) &&(choice != OPEND_DOOR));

	if(choice==OPEND_DOOR){
		/*
		 * 1-ask to enter the password then sent it to mc2 to check it
		 * 2-if correct send open_door_cmd to mc2
		 * 3-display message on screen for current statuses
		 * 4-if not correct send play_buzzer_cmd to mc2
		 */

		if(get_and_check_password()){
			UART_sendByte(OPEND_DOOR);
			LCD_clearScreen();
			LCD_displayString("Door is opening");
			delay(15000);
			LCD_clearScreen();
			LCD_displayString("Door opened");
			delay(3000);
			LCD_clearScreen();
			LCD_displayString("Door is closing");
			delay(15000);
		}
		else{
			UART_sendByte(PLAY_BUZZER);
			LCD_clearScreen();
			LCD_displayString("Warning!");
			delay(60000);
		}
		//after finish clear lcd and return again to mc1 home screen
		LCD_clearScreen();
	}
	else if(choice=='-'){
		/*enter the old password then sent to mc2 to check it
		 *if password is correct  call set_password function on mc1 to get input from user
		 *if not correct ask him to input it again
		 *if input is not correct for 3 times ask MC2 to play buzzer
		 *after finishing on set_password function return to home screen again
		 */
		if(get_and_check_password()){
			UART_sendByte(SET_PASSWORD);
			set_password();
		}
		else{
			UART_sendByte(PLAY_BUZZER);
			LCD_clearScreen();
			LCD_displayString("Warning!");
			delay(3000);
		}
		//after finish clear lcd and return again to mc1 home screen
		LCD_clearScreen();
	}
}

/*for getting password from the user and send it to MC2 to check whether it is correct or not*/
uint8 get_and_check_password(){
	uint8 key,input_time=0,result;
	do{
		UART_sendByte(VALIDATE_PASSWORD);
		input_time++;
		LCD_clearScreen();
		LCD_moveCursor(0,0);
		LCD_displayString("Enter password");
		for(uint8 num=0;num<6;num++){
			key=KEYPAD_getPressedKey();
			if(key!=13 && num!=5){
				password[num]=key+'0';
				LCD_moveCursor(1,num);
				LCD_displayCharacter('*');
			}
			else if (key==13 && num==5){
				password[num]='#';
				password[num+1]='\0';
				UART_sendString(password);
				LCD_clearScreen();
			}
			else
				--num;
			delay(200);
		}
		result=UART_recieveByte();
		if(result=='0'){
			LCD_displayString("Not Correct!");
			delay(1000);
		}
	}while(result != '1' && input_time!=3);
	if((result!= '1') && input_time==3)
		//return 0 to turn on buzzer for 1min and display warning message on LCD;
		return 0;
	else
		//return 1 to inform the caller that this password is correct
		return 1;
}
