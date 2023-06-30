 /******************************************************************************
 *
 * Module: DcMotor
 *
 * File Name: dcmotor.h
 *
 * Description: Header file for the DcMotor driver
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define DC_MOTOR_IN1_PORT_ID 		PORTB_ID
#define DC_MOTOR_IN1_PIN_ID 		PIN0_ID
#define DC_MOTOR_IN2_PORT_ID		PORTB_ID
#define DC_MOTOR_IN2_PIN_ID			PIN1_ID
#define DC_MOTOR_EN_PORT_ID			PORTB_ID
#define DC_MOTOR_EN_PIN_ID			PIN3_ID

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
/*This enum represents the state of the motor and it's used in the
 * DcMotor_Rotate() function
 */
typedef enum{
	STOP,CW,CCW
}DcMotor_State;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * This function is responsible for setting up the direction of the two motor pins
 * via GPIO driver.
 * It also configure the motor to be off at the beginning via GPIO driver.
 */
void DcMotor_init(void);

/*
 * Description :
 * This function is responsible for the state of the motion based on the input
 * state: STOP, CW, or CCW.
 * It also handles the speed of the motor via sending the required speed
 * percentage to the PWM driver to output the required analog signal
 * to the motor.
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DCMOTOR_H_ */
