/******************************************************************************
 *
 * Title: 		Door Locker System (Control ECU)
 *
 * Description: Application code for the HMI ECU
 *
 * Author:		Mohamed Gad
 *
 * Date: 		23/6/2023
 *
 *******************************************************************************/
/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "MC1.h"
#include <avr/io.h> /*To access SREG */
#include <util/delay.h> /*To use delay functions*/
#include "lcd.h"
#include "keypad.h"
#include "timer1.h"
#include "uart.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*Flag that signals the completion of Timer 1 task in the code*/
boolean g_timer1_done=FALSE;

/*Flag that indicates whether the timer is done with the delay or not*/
boolean g_timer1_delay=FALSE;

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
	/*Configuration Structure For UART*/
	UART_ConfigType UART_Config_Struct={
			No_Parity,ONE_BIT,EIGHT_BITDATA,9600
	};

	/*Variable that stores the status of the checked password (Passed, Failed,or Thief)*/
	Password_Status Password_State;

	/*
	 * Flag responsible for executing or skipping the snippet of code that is responsible for reading and checking
	 * The password entered by the user.
	 */
	boolean Password_Step_Flag=TRUE;

	/*
	 * Flag responsible for executing or skipping the snippet of code that is responsible for displaying the main
	 * options and take input from the user. (+: open door, -: change password).
	 */
	boolean Options_step_flag=TRUE;

	/*Variable to store the input taken from the user in the main options stage*/
	uint8 Option_choice;

	/*
	 * String to store the password entered in the first attempt.
	 * Note: It's of size 7 because it stores 5 chars of the password and the '#' to be able to send it via UART.
	 */
	uint8 pass_one[7];

	/*
	 * String to store the password entered in the second attempt.
	 * Note: It's of size 7 because it stores 5 chars of the password and the '#' to be able to send it via UART.
	 */
	uint8 pass_two[7];

	/********************HARDWARE INITIALIZATIONS********************/
	LCD_init();
	UART_init(&UART_Config_Struct);

	/*Enable The global interrupts (I-bit)*/
	SREG|=(1<<7);

	/********************PROGRAM LOGIC********************/
	while(1)
	{
		/*Step 1: Receive the 2 passwords entered by the user from HMI ECU*/
		while(Password_Step_Flag==TRUE)
		{
			/*Enter the first password*/
			Enter_Password(pass_one,TRUE);

			/*Enter the second password*/
			Enter_Password(pass_two, FALSE);

			/*Send the first password to the Control ECU*/
			UART_sendString(pass_one);

			/*Hardware delay*/
			_delay_ms(25);

			/*Send the second password to the Control ECU*/
			UART_sendString(pass_two);

			/*Receive whether the two passwords match or not from the Control ECU*/
			Password_State=UART_recieveByte();

			if(Password_State==PASSWORD_PASSED)
			{
				/*If the two passwords are matched, disable step one flag (passStep_flag)*/
				Password_Step_Flag=FALSE;

				/*Enable the second step flag*/
				Options_step_flag=TRUE;
			}
			else
			{
				/*If the two passwords aren't matched, stay at step 1*/
				Password_Step_Flag=TRUE;
			}
		}

		/*Step 2: display the main option to the user and get the input from him (+: Open door, -: change password_*/
		while(Options_step_flag==TRUE)
		{

			/*Display the main options and get the input*/
			Option_choice=Choice_Option_menu();

			if(Option_choice=='+')
			{
				/*If the user wants to open the door, prompt him to enter the password*/
				Enter_Password(pass_one, TRUE);

				/*Send the user choice to the Control ECU*/
				UART_sendByte('+');

				/*Hardware delay*/
				_delay_ms(25);

				/*Send the entered password to the Control ECU to be compared with the one saved in the EEPROM*/
				UART_sendString(pass_one);

				/*Receive whether the password is correct or not*/
				Password_State=UART_recieveByte();

				while(Password_State==PASSWORD_FAILED)
				{
					LCD_clearScreen();
					LCD_displayString("Wrong Password");
					_delay_ms(1000);

					/*If the password isn't correct, enter the password again; the user has three attempts*/
					Enter_Password(pass_one, TRUE);

					/*Hardware delay*/
					_delay_ms(25);

					/*Send the entered password to the Control ECU to be compared with the one saved in the EEPROM*/
					UART_sendString(pass_one);

					/*Receive whether the password is correct or not*/
					Password_State=UART_recieveByte();
				}

				if(Password_State==PASSWORD_LOCKED)
				{
					/*If the user failed to enter the correct password three consecutive times, display an error message*/
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 5, "ERROR!");

					/*Display the error message for 1 minute*/
					/*Start the timer to count 1 minute*/
					Timer1_setCallBack(callBack);
					Timer1_init(&Timer1_Delay_Struct);
					g_timer1_delay=TRUE;

					/*Wait until the timer is done*/
					while(g_timer1_delay==TRUE);

					/*Continue to return to the main options menu again*/
					continue;
				}
				else if(Password_State==PASSWORD_PASSED)
				{
					/*Else if the the password is correct, start the timer to display the door state (unlocking, unlocked, locking)*/
					Timer1_setCallBack(callBack);
					Timer1_init(&Timer1_Config_Struct);

					/*Wait until the door is done opening and closing, .i.e, the time has finished*/
					while(!g_timer1_done);

					/*Reset the timer flag*/
					g_timer1_done=FALSE;

					/*Continue to return to the main options menu again*/
					continue;
				}
				else
				{
					/*Do Nothing*/
				}
			}
			else  if(Option_choice=='-')
			{
				/*If the user want to change the password, the user is prompted to enter the saved password first*/
				Enter_Password(pass_one, TRUE);

				/*Send the user choice to the Control ECU*/
				UART_sendByte('-');

				/*Hardware delay*/
				_delay_ms(25);

				/*Send the entered password to the control ECU to be checked*/
				UART_sendString(pass_one);

				/*Receive whether the password is correct or not*/
				Password_State=UART_recieveByte();

				while(Password_State==PASSWORD_FAILED)
				{
					LCD_clearScreen();
					LCD_displayString("Wrong Password");
					_delay_ms(1000);

					/*If the password isn't correct, enter the password again; the user has three attempts*/
					Enter_Password(pass_one, TRUE);

					/*Hardware delay*/
					_delay_ms(25);

					/*Send the entered password to the control ECU to be checked*/
					UART_sendString(pass_one);

					/*Receive whether the password is correct or not*/
					Password_State=UART_recieveByte();
				}

				if(Password_State==PASSWORD_LOCKED)
				{
					/*If the user failed to enter the correct password three consecutive times, display an error message*/
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 5, "ERROR!");

					/*Display the error message for 1 minute*/
					/*Start the timer to count 1 minute*/
					Timer1_setCallBack(callBack);
					Timer1_init(&Timer1_Delay_Struct);
					g_timer1_delay=TRUE;

					/*Wait until the timer is done*/
					while(g_timer1_delay==TRUE);

					/*Continue to return to the main options menu again*/
					continue;
				}
				else if(Password_State==PASSWORD_PASSED)
				{
					LCD_clearScreen();
					LCD_displayString("Correct Password");
					LCD_displayStringRowColumn(1, 0, "Reset Password");
					_delay_ms(1000);

					/*If the password is correct, enable step one flag to return to reset password step*/
					Password_Step_Flag=TRUE;

					/*Disable step 2 flag for now*/
					Options_step_flag=FALSE;
				}
				else
				{
					/*Do Nothing*/
				}
			}
			else
			{
				/*Do Nothing*/
			}
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * This function display a prompt to the user to enter the password whether to save it or to store it in EEPROM.
 */
void Enter_Password(uint8* pass,boolean isFirst)
{
	/*Variable to index the characters in the password to be saved*/
	uint8 pass_counter=0;

	/*Variable to store the ascii of the pressed key on the keypad*/
	uint8 Key;

	if(isFirst==1)
	{
		/*If this is password 1, display messages related to this password*/
		LCD_clearScreen();
		LCD_displayString("Plz Enter Pass: ");
		LCD_moveCursor(1, 0);
	}
	else
	{
		/*Else if this is password 2, prompt the user to reenter the same password entered at first*/
		LCD_clearScreen();
		LCD_displayString("Plz Re-Enter the");
		LCD_moveCursor(1, 0);
		LCD_displayString("Same Pass:");
	}

	/*Loop to get and store the entered password using keypad*/
	for(pass_counter=0;pass_counter<PASSWORD_LIMIT;pass_counter++)
	{
		/*Get ascii of the pressed key*/
		Key=KEYPAD_getPressedKey();

		if(Key=='=')
		{
			/*If the user pressed enter, add the '#' at the end of the string to be able to send it via UART to the Control ECU*/
			pass[pass_counter]='#';

			/*Lastly, add the null terminator '\0'*/
			pass[pass_counter+1]='\0';

			/*End the loop*/
			break;
		}

		if(pass_counter==5)
		{
			/*At character number 6, wait for the user to press enter ('=')*/
			if(Key=='=')
			{
				/*If the user pressed enter, add the '#' at the end of the string to be able to send it via UART to the Control ECU*/
				pass[pass_counter]='#';

				/*Lastly, add the null terminator '\0'*/
				pass[pass_counter+1]='\0';

				/*End the loop*/
				break;
			}
			else
			{
				/*If the user pressed any key other than enter, reduce the counter then continue, until the user hits enter*/
				pass_counter=4;
				continue;
			}
		}
		else
		{
			/*Save the entered characters in the password string*/
			pass[pass_counter]=Key;

			/*Display '*' on the LCD with each character taken from user */
			LCD_displayCharacter('*');
		}

		/*Hardware delay*/
		_delay_ms(500);
	}
}

/*
 * Description :
 * This function display the main options on the LCD, and takes input from the user then return that input.
 */
uint8 Choice_Option_menu(void)
{
	/*Variable to store ascii of pressed key, and boolean to control the while loop*/
	uint8 pressed_key,bool=TRUE;

	/*To store the pressed key again, but this one is used in return statement*/
	uint8 choice;

	/*Display main options on LCD*/
	LCD_clearScreen();
	LCD_displayString("+ : Open Door");
	LCD_moveCursor(1, 0);
	LCD_displayString("- : Change Pass");

	while(bool)
	{
		/*Get the key*/
		pressed_key=KEYPAD_getPressedKey();
		switch (pressed_key)
		{
		case '+':
			/*get out of the loop only if user chooses '+' or '-'*/
			bool=FALSE;
			choice= '+';
			break;
		case '-':
			bool=FALSE;
			choice='-';
			break;
		default:
			/*If any thing other than '+' and  '-' button is pressed, do nothing*/
			bool=TRUE;
			break;
		}
		/*Hardware delay*/
		_delay_ms(100);
	}

	/*Return ascii of the pressed key*/
	return choice;
}

/*
 * Description :
 * This is the callBack Function that'll be called when an interrupt request is generated
 * by Timer1.
 */
void callBack(void)
{
	static sint8 state_counter=1;

	/*Ask whether we want to use timer as delay to display error message, or use it to count time of motor opening and closing*/
	if(g_timer1_delay==TRUE)
	{
		/*If the delay is done, reset everything*/
		g_timer1_delay=FALSE;
		Timer1_deInit();
	}
	else
	{
		switch (state_counter)
		{
		case 1:
			/*Configure timer to count 15 seconds, during which "Door is unlocking" is displayed on LCD*/
			state_counter++;
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 5, "Door is ");
			LCD_displayStringRowColumn(1, 5, "Unlocking");
			Timer1_Config_Struct.mode=TIMER1_COMPARE_MODE;
			Timer1_Config_Struct.prescaler=F_CPU_1024;
			Timer1_Config_Struct.compare_value=14649;
			Timer1_Config_Struct.initial_value=0;
			Timer1_init(&Timer1_Config_Struct);
			break;
		case 2:
			/*Configure timer to count 3 seconds, during which "Door is unlocked" is displayed on LCD*/
			state_counter++;
			LCD_clearScreen();
			LCD_displayString("Door Unlocked");
			Timer1_Config_Struct.compare_value=2930;
			Timer1_Config_Struct.initial_value=0;
			Timer1_init(&Timer1_Config_Struct);
			break;
		case 3:
			/*Configure timer to count 15 seconds, during which "Door is locking" is displayed on LCD*/
			state_counter++;
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 5, "Door is ");
			LCD_displayStringRowColumn(1, 5, "Locking");
			Timer1_Config_Struct.compare_value=14649;
			Timer1_Config_Struct.initial_value=0;
			Timer1_init(&Timer1_Config_Struct);
			break;
		case 4:
			/*Reset everything and deinit the timer*/
			state_counter=1;

			/*Dummy configurations to start the timer with in the next time*/
			Timer1_Config_Struct.compare_value=1;
			Timer1_Config_Struct.initial_value=0;
			Timer1_Config_Struct.mode=TIMER1_COMPARE_MODE;
			Timer1_Config_Struct.prescaler=NO_PRESCALER;

			/*Signal that timer has finished to get out of the infinite loop*/
			g_timer1_done=TRUE;
			Timer1_deInit();
			break;
		}
	}
}
