/******************************************************************************
 *
 * Title: 		Door Locker System (Control ECU)
 *
 * Author:		Mohamed Gad
 *
 * Date: 		25/6/2023
 *
 *******************************************************************************/
/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include "CONTROL_ECU.h"
#include "dcmotor.h"
#include "external_eeprom.h"
#include "buzzer.h"
#include "timer1.h"
#include "uart.h"
#include "twi.h"
#include <string.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*Flag that signals the completion of Timer 1 task in the code*/
boolean g_timer_is_finished=FALSE;

/*Flag that indicates whether the timer is done with the delay or not*/
boolean g_timer_delay_flag=FALSE;

/*Dummy Configuration Structure For TIMER1*/
Timer1_ConfigType Timer1_Config_Struct={
		0,1,NO_PRESCALER,TIMER1_COMPARE_MODE
};

/*Timer configuration structure to be used as a delay*/
Timer1_ConfigType Timer1_Delay_Struct={
		0,58594,F_CPU_1024,TIMER1_COMPARE_MODE
};
/*******************************************************************************
 *                      Main Function Definition                               *
 *******************************************************************************/

int main(void)
{
	/*Configuration Structure For I2C*/
	TWI_ConfigType TWI_Config_Struct={
			1,FAST_MODE
	};

	/*Configuration Structure For UART*/
	UART_ConfigType UART_Config_Struct={
			PARITY_DISABLED,ONE_STOP,EIGHT_BITS,9600
	};

	/*Variable that stores the status of the checked password (Passed, Failed,or Thief)*/
	Password_Status pass_state;

	/*
	 * Flag responsible for executing or skipping the snippet of code that is responsible for reading and checking
	 * The password entered by the user.
	 */
	boolean passStep_flag=TRUE;

	/*String to store the password entered in the first attempt*/
	uint8 pass_one[6];

	/*String to store the password entered in the second attempt*/
	uint8 pass_two[6];

	/*Variable used to store the choice entered by the user from the Main Options (open door, change password)*/
	uint8 optionStep_choice;

	/*Counter to count the number of failed attempts during entering the password to open the door or changing the password*/
	uint8 wrongPass_counter=0;

	/********************HARDWARE INITIALIZATIONS********************/
	UART_init(&UART_Config_Struct);
	TWI_init(&TWI_Config_Struct);
	DcMotor_init();
	Buzzer_init();

	/*Enable The global interrupts (I-bit)*/
	SREG|=(1<<7);

	/********************PROGRAM LOGIC********************/
	while(1)
	{
		/*Step 1: Receive and check the 2 passwords entered by the user from HMI ECU*/
		while(passStep_flag==TRUE)
		{
			/*Check the passwords after receiving them*/
			pass_state=receiveAndCheckPassword(pass_one,pass_two);

			if(pass_state==PASSWORD_PASSED)
			{
				/*If the passwords are matched, alert the HMI ECU*/
				UART_sendByte(pass_state);

				/*Save the password in the EEPROM*/
				savePassword(pass_one);

				/*Disable the flag to break out of the loop*/
				passStep_flag=FALSE;
			}
			else if(pass_state==PASSWORD_FAILED)
			{
				/*If the passwords are not matched, alert the HMI ECU to prompt the user to retry again*/
				UART_sendByte(pass_state);
			}
			else
			{
				/*Do Nothing*/
			}
		}

		/*Step 2: Receive the choice picked by the user from the Main Options (+: open door, -: change password)*/
		optionStep_choice=UART_recieveByte();

		if(optionStep_choice=='+')
		{

			/*If the user wants to open the door, ask him to enter the saved password in the EEPROM*/
			do{
				/*Receive the password entered by the user*/
				UART_receiveString(pass_one);

				/*Check this password*/
				pass_state=confirmPassword(pass_one);

				if(pass_state==PASSWORD_FAILED)
				{
					/*Increment the wrong password counter, because if the user fails 3 times, the buzzer rings*/
					wrongPass_counter++;

					if(wrongPass_counter==3)
					{
						/*
						 * If the user entered wrong password 3 consecutive times:
						 * 1- Alert HMI ECU to display Error message on LCD.
						 */
						pass_state=THIEF;

						/*Alert the HMI that the user failed for the third time to display the error message*/
						UART_sendByte(pass_state);

						break;
					}
					/*If the password is wrong, alert the HMI ECU to prompt the user to retry*/
					UART_sendByte(pass_state);
				}
				else if(pass_state==PASSWORD_PASSED)
				{
					/*If the user attempt was a success, reset the attempts counter*/
					wrongPass_counter=0;

					/*Alert the HMI ECU that every thing is fine*/
					UART_sendByte(pass_state);

					/*Break out of the loop*/
					break;
				}
				else
				{
					/*Do Nothing*/
				}
			}while(wrongPass_counter<3);


			if(wrongPass_counter==3)
			{
				/*
				 * If the user entered wrong password 3 consecutive times:
				 * 1- Alert HMI ECU to display Error message on LCD.
				 * 2- Activate the buzzer for 1 Minute
				 */
				Buzzer_on();

				/*Start the timer to count 1 minute*/
				Timer1_setCallBack(callBack);
				Timer1_init(&Timer1_Delay_Struct);
				g_timer_delay_flag=TRUE;

				/*Wait until the timer is done*/
				while(g_timer_delay_flag==TRUE);

				/*Turn Off The Buzzer*/
				Buzzer_off();
			}
			else if(pass_state==PASSWORD_PASSED)
			{
				/*
				 * If the user entered the correct password, initiate the timer with dummy configurations to
				 * call the callBack function that controls the opening and closing of the motor.
				 */
				Timer1_setCallBack(callBack);
				Timer1_init(&Timer1_Config_Struct);

				/*Wait until the timer is done opening and closing the door (MOTOR)*/
				while(!g_timer_is_finished);

				/*Reset the flag*/
				g_timer_is_finished=FALSE;
			}
			else
			{
				/*Do Nothing*/
			}

			/*Reset the counter for next checks.*/
			wrongPass_counter=0;
		}
		else if(optionStep_choice=='-')
		{
			/*If the user wants to change the password, ask him to enter the saved password in the EEPROM*/
			do{
				/*Receive the entered password*/
				UART_receiveString(pass_one);

				/*Confirm this password via comparing it with the one saved in the EEPROM*/
				pass_state=confirmPassword(pass_one);

				if(pass_state==PASSWORD_FAILED)
				{
					/*Increment the wrong password counter, because if the user fails 3 times, the buzzer rings*/
					wrongPass_counter++;

					if(wrongPass_counter==3)
					{
						/*
						 * If the user entered wrong password 3 consecutive times:
						 * 1- Alert HMI ECU to display Error message on LCD.
						 */
						pass_state=THIEF;

						/*Alert the HMI that the user failed for the third time to display the error message*/
						UART_sendByte(pass_state);

						break;
					}
					/*If the password is wrong, alert the HMI ECU to prompt the user to retry*/
					UART_sendByte(pass_state);
				}
				else if(pass_state==PASSWORD_PASSED)
				{
					/*If the user attempt was a success, reset the attempts counter*/
					wrongPass_counter=0;

					/*If the password is correct, inform the HMI ECU and break out of the loop*/
					UART_sendByte(pass_state);

					/*Break out of the loop*/
					break;
				}
			}while(wrongPass_counter<3);

			if(wrongPass_counter==3)
			{
				/*
				 * If the user entered wrong password 3 consecutive times:
				 * 1- Alert HMI ECU to display Error message on LCD.
				 * 2- Activate the buzzer for 1 Minute
				 */
				Buzzer_on();

				/*Start the timer to count 1 minute*/
				Timer1_setCallBack(callBack);
				Timer1_init(&Timer1_Delay_Struct);
				g_timer_delay_flag=TRUE;

				/*Wait until the timer is done*/
				while(g_timer_delay_flag==TRUE);

				/*Turn Off The Buzzer*/
				Buzzer_off();
			}
			else if(pass_state==PASSWORD_PASSED)
			{
				/*
				 * If the password is correct, then the user is permitted to change the password, thus we return to the first step
				 * which is changing the current password and saving the new password.
				 */

				/*Enable the flag responsible for executing step one*/
				passStep_flag=TRUE;
			}
			else
			{
				/*Do Nothing*/
			}

			/*Reset the counter for next checks.*/
			wrongPass_counter=0;
		}
		else
		{
			/*Do Nothing*/
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * This function receives the two passwords entered by the user from HMI_ECU, and
 * compare them to see whether they match or not.
 */
Password_Status receiveAndCheckPassword(uint8*pass_one,uint8*pass_two)
{
	/*Receive the entered passwords from HMI ECU*/
	UART_receiveString(pass_one);
	UART_receiveString(pass_two);

	if(strcmp((const char*)pass_one,(const char*)pass_two)==0)
	{
		/*If the 2 passwords are matched, return PASSWORD_PASSED*/
		return PASSWORD_PASSED;
	}
	else
	{
		/*If the 2 passwords aren't matched, return PASSWORD_FAILED*/
		return PASSWORD_FAILED;
	}
}

/*
 * Description :
 * This function saves the validated password in the external EEPROM.
 */
void savePassword(const uint8*pass)
{
	/*Variable to index the characters in the password to be saved*/
	uint8 pass_counter=0;

	/*Starting address in the EEPROM to save the password in*/
	uint16 address=0x0010;

	/*Save the password in the EEPROM byte by byte*/
	for(pass_counter=0,address=0x0010; pass_counter<6 ; pass_counter++,address++)
	{
		EEPROM_writeByte(address, pass[pass_counter]);

		/*Hardware delay*/
		_delay_ms(10);
	}
}

/*
 * Description :
 * This function reads the saved password from the external EEPROM.
 */
void retrievePassword(uint8* pass)
{
	/*Variable to index the characters in the password to be retrieved*/
	uint8 pass_counter=0;

	/*Starting address in the EEPROM to retrieve the password from*/
	uint16 address=0x0010;

	/*Retrieve the password from the EEPROM byte by byte*/
	for(pass_counter=0,address=0x0010; pass_counter<6 ; pass_counter++,address++)
	{
		EEPROM_readByte(address, &pass[pass_counter]);

		/*Hardware delay*/
		_delay_ms(10);
	}
}

/*
 * Description :
 * This function compares the input password with the one save in the EEPROM,
 * and return whether the password is correct or not.
 */
Password_Status confirmPassword(uint8*pass_one)
{
	/*String to store the retrieved password from EEPROM*/
	uint8 eeprom_pass[6];

	/*retrieve the password*/
	retrievePassword(eeprom_pass);

	/*Compare to check whether they match or not*/
	if(strcmp((const char*)pass_one,(const char*)eeprom_pass)==0)
	{

		/*If the 2 passwords are matched, return PASSWORD_PASSED*/
		return PASSWORD_PASSED;
	}
	else
	{
		/*If the 2 passwords aren't matched, return PASSWORD_FAILED*/
		return PASSWORD_FAILED;
	}
}

/*
 * Description :
 * This is the callBack Function that'll be called when an interrupt request is generated
 * by Timer1.
 */
void callBack(void)
{
	/*Interrupt counter, used in time calculations*/
	static uint8 interrupt_counter=0;

	/*A variable to store which stage of motor rotation we'r in*/
	static sint8 state_counter=1;

	/*Flag to execute the initializations of the timer only once when this function is first called*/
	static boolean init1_flag=TRUE;

	/*Ask whether we want to use timer as delay to activate buzzer, or use it to count time of motor opening and closing*/
	if(g_timer_delay_flag==TRUE)
	{
		/*If we use timer as delay increment the counter till it reaches 8, .i.e, a minute has passed*/
		interrupt_counter++;
		if(interrupt_counter==8)
		{
			/*After one minute, reset every thing*/

			/*Turn off the flag to stop the polling in the main program*/
			g_timer_delay_flag=FALSE;
			Timer1_deInit();
			interrupt_counter=0;
		}

	}
	else
	{
		/*switch to execute the proper stage code*/
		switch (state_counter)
		{
		case 1:
			if(init1_flag==TRUE)
			{
				/*If the flag is 1, it means it's the first time to call this function, hence we need to execute the initializations*/

				/*Turn off the flag, to prevent multiple configuration setup*/
				init1_flag=FALSE;

				/*start the motor, .i.e, open the door*/
				DcMotor_Rotate(CW, 100);

				/*start the timer to count 15 seconds*/
				Timer1_Config_Struct.mode=TIMER1_COMPARE_MODE;
				Timer1_Config_Struct.prescaler=F_CPU_1024;
				Timer1_Config_Struct.compare_value=60000;
				Timer1_Config_Struct.initial_value=0;
				Timer1_init(&Timer1_Config_Struct);

				/*Break out of the switch after configuring the timer for the first time*/
				break;
			}
			else
			{
				/*Do Nothing*/
			}

			/*increment the counter because 15 seconds means approximately 2 compare matches of the timer*/
			interrupt_counter++;

			if(interrupt_counter==2)
			{
				/*After 15 seconds, reset the counter and turn off the motor*/
				interrupt_counter=0;
				DcMotor_Rotate(STOP, 0);

				/*change motor state, hold for 3 seconds*/
				state_counter++;

				/*configure the timer to count 3 seconds*/
				Timer1_Config_Struct.compare_value=23438;
				Timer1_Config_Struct.initial_value=0;
				Timer1_init(&Timer1_Config_Struct);
			}
			else
			{
				/*Do Nothing*/
			}
			break;
		case 2:
			/*After 3 seconds, rotate the motor in the opposite direction for 15 more seconds*/
			/*reset the counter*/
			interrupt_counter=0;
			DcMotor_Rotate(CCW, 100);

			/*Increment to call back reset stage*/
			state_counter++;

			/*Configure the timer to count 15 seconds*/
			Timer1_Config_Struct.compare_value=60000;
			Timer1_Config_Struct.initial_value=0;
			Timer1_init(&Timer1_Config_Struct);
			break;
		case 3:
			/*increment the counter because 15 seconds means approximately 2 compare matches of the timer*/
			interrupt_counter++;

			if(interrupt_counter==2)
			{
				/*After 15 seconds, .i.e, door is closed, reset the call back function for next usage*/
				interrupt_counter=0;

				/*Turn off the motor*/
				DcMotor_Rotate(STOP, 0);

				/*Back to state 1*/
				state_counter=1;

				/*Re-enable first configuration flag*/
				init1_flag=TRUE;

				/*Dummy configurations*/
				Timer1_Config_Struct.compare_value=1;
				Timer1_Config_Struct.initial_value=0;
				Timer1_Config_Struct.mode=TIMER1_COMPARE_MODE;
				Timer1_Config_Struct.prescaler=NO_PRESCALER;

				/*Global timer flag is enabled to signal that the program is permitted to execute other instructions, .i.e, system unlocked*/
				g_timer_is_finished=TRUE;

				/*Disable the timer*/
				Timer1_deInit();
			}
			else
			{
				/*Do Nothing*/
			}
			break;
		}
	}
}
