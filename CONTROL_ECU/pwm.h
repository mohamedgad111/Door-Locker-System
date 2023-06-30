/******************************************************************************
 *
 * Module: PWM Timer0 For DC Motor
 *
 * File Name: pwm.h
 *
 * Description: Header file for the AVR PWM Timer0 driver
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

#include "std_types.h"

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * This Function is responsible for:
 * 1- Setting up Timer0 with PWM mode.
 * 2- setting PWM with non-inverting mode.
 * 3- Timer prescaler.
 * 4- Initializing the compare value based on the input duty cycle.
 * 5- Configure the direction of the OC0 pin.
 * 6- Setting the PWM frequency to be 500 HZ to be compatible with motor
 *    operational frquency.
 * Function inputs: Required duty cycle.
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* PWM_H_ */
