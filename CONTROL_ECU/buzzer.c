/******************************************************************************
 *
 * Module: Buzzer Module
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the buzzer module
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "buzzer.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * 1) This function is responsible for setup the direction of the buzzer
 *    control pin.
 * 2) Turn off the buzzer at first, .i.e, OUTPUT LOGIC LOW.
 */
void Buzzer_init(void)
{
	/*Setup the direction of the buzzer control pin*/
	GPIO_setupPinDirection(BUZZER_CNTRL_PORT_ID, BUZZER_CNTRL_PIN_ID, PIN_OUTPUT);

	/*Turn off the buzzer*/
	GPIO_writePin(BUZZER_CNTRL_PORT_ID, BUZZER_CNTRL_PIN_ID, BUZZER_OFF);
}

/*
 * Description :
 * Function to enable the buzzer through the GPIO.
 */
void Buzzer_on(void)
{
	/*Turn on the buzzer*/
	GPIO_writePin(BUZZER_CNTRL_PORT_ID, BUZZER_CNTRL_PIN_ID, BUZZER_ON);
}

/*
 * Description :
 * Function to disable the buzzer through the GPiO.
 */
void Buzzer_off(void)
{
	/*Turn off the buzzer*/
	GPIO_writePin(BUZZER_CNTRL_PORT_ID, BUZZER_CNTRL_PIN_ID, BUZZER_OFF);
}
