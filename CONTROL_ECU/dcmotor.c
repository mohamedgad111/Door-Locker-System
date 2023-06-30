/******************************************************************************
 *
 * Module: DcMotor
 *
 * File Name: dcmotor.c
 *
 * Description: Source file for the DcMotor driver
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/
#include "dcmotor.h"
#include "pwm.h" /*To be able to control the motor speed via generating a suitable waveform*/

/*
 * Description :
 * This function is responsible for setting up the direction of the two motor pins
 * via GPIO driver.
 * It also configure the motor to be off at the beginning via GPIO driver.
 */
void DcMotor_init(void)
{
	/*Setup the two control pins of the motor as output pins*/
	GPIO_setupPinDirection(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,PIN_OUTPUT);

	/*Output 0 to the two pins to STOP the motor*/
	GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);

}
/*
 * Description :
 * This function is responsible for the state of the motion based on the input
 * state: STOP, CW, or CCW.
 * It also handles the speed of the motor via sending the required speed
 * percentage to the PWM driver to output the required analog signal
 * to the motor.
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	/*Output the state to the two control pins of the motor
	 * 0 -> STOP
	 * 1 -> CW
	 * 2 -> CCW
	 */
	switch (state)
	{
	case 0:
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
		break;
	case 1:
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
		break;
	case 2:
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
		break;
	default:
		/*Do Nothing*/
		break;
	}

	/*Send required speed to Timer0 to generate the required PWM on OC0 pin*/
	PWM_Timer0_Start(speed);

}
