/******************************************************************************
 *
 * Module: Timer1 Driver
 *
 * File Name: timer1.h
 *
 * Description: Header file for AVR Timer1 Driver
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

#ifndef TIMER1_H_
#define TIMER1_H_

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum{
	NO_PRESCALER=1,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}Timer1_Prescaler;

typedef enum{
	TIMER1_NORMAL_MODE,TIMER1_COMPARE_MODE
}Timer1_Mode;

typedef struct {
	uint16 initial_value;
	uint16 compare_value;
	Timer1_Prescaler prescaler;
	Timer1_Mode mode;
} Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function to initialize the Timer driver.
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*
 * Description :
 * Function to disable the Timer driver.
 */
void Timer1_deInit(void);

/*
 * Description :
 * Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void));

#endif /* TIMER1_H_ */
