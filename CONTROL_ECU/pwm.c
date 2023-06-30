/******************************************************************************
 *
 * Module: PWM Timer0 For DC Motor
 *
 * File Name: pwm.c
 *
 * Description: Source file for the AVR PWM Timer0 driver
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/
#include "pwm.h"
#include "gpio.h"/*To set pin directions*/
#include <avr/io.h> /*To use Timer0 registers*/
/*
 * Description :
 * This Function is responsible for:
 * 1- Setting up Timer0 with PWM mode.
 * 2- setting PWM with non-inverting mode.
 * 3- Timer prescaler.
 * 4- Initializing the compare value based on the input duty cycle.
 * 5- Configure the direction of the OC0 pin.
 * 6- Setting the PWM frequency to be 500 HZ to be compatible with motor
 *    operational frequency.
 * Function inputs: Required duty cycle.
 */
void PWM_Timer0_Start(uint8 duty_cycle)
{
	/*Variable to store the required OCR0 register value*/
	uint16 req_OCR0_value=0;

	/*Flag to set TCNT0 register only once.
	 * It's static to initialize it with the first call of the function only, then keep
	 * its value with further call of this function.
	 */
	static uint8 TCNT0_flag= 1;

	/*Set OC0 as output pin*/
	GPIO_setupPinDirection(PORTB_ID,PIN3_ID,PIN_OUTPUT);

	/*Initialize TCNT0*/
	if(TCNT0_flag== 1)
	{
		TCNT0_flag= 0;
		TCNT0=0;
	}

	/*Calculating the equivalent OCR register value base on the input duty cycle
	  percentage*/
	req_OCR0_value=(uint16)(((float32)duty_cycle/100.0)*256);

	/*Check for overflow, i.e, 100% duty cycle*/
	if(req_OCR0_value==256)
	{
		/*Decrement the value by one, in case of overflow, to be able
		 * to store this value in OCR0, which is an 8-bit register.
		 */
		req_OCR0_value--;
	}

	/*assign the required OCR0 value*/
	OCR0=(uint8)req_OCR0_value;



	/*Configure The control bits in TCCR0 register to work with PWM mode.
	 * FOC0=0 -> PWM-mode
	 * WGM00=WGM01=1 -> Fast PWM
	 * COM01=1 / COM00=0 -> non=inverting mode
	 * CS01=1 / CS00=0 / CS02=0 -> prescaler F_CPU/8
	 */
	TCCR0=(1<<WGM00)|(1<<WGM01)|(1<<CS01)|(1<<COM01);

}


