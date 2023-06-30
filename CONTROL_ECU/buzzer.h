 /******************************************************************************
 *
 * Module: Buzzer Module
 *
 * File Name: buzzer.h
 *
 * Description: Header file for Buzzer
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define BUZZER_CNTRL_PORT_ID		PORTD_ID
#define BUZZER_CNTRL_PIN_ID			PIN6_ID

#define BUZZER_ON					LOGIC_HIGH
#define BUZZER_OFF					LOGIC_LOW

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * 1) This function is responsible for setup the direction of the buzzer
 *    control pin.
 * 2) Turn off the buzzer at first, .i.e, OUTPUT LOGIC LOW.
 */
void Buzzer_init(void);

/*
 * Description :
 * Function to enable the buzzer through the GPIO.
 */
void Buzzer_on(void);

/*
 * Description :
 * Function to disable the buzzer through the GPiO.
 */
void Buzzer_off(void);

#endif /* BUZZER_H_ */
