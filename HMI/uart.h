 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/*CPU Clock Frequency, used in calculating The baud rate of the UART to be stored in UBRR register.*/
#define UART_F_CPU 1000000UL

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/


typedef enum
{
	No_Parity,
	EVEN_Parity=2,
	Odd_Parity=3
}UART_Parity;


typedef enum
{
	FIVE_BITDATA,
	SIX_BITDATA,
	SEVEN_BITDATA,
	EIGHT_BITDATA,
	NINE_BITDATA=7
}UART_BITDATA;


typedef enum
{
	ONE_BIT,
	TWO_BITS
}UART_STOPBIT;


typedef uint32 UART_BaudRate;

typedef struct
{
	UART_Parity  parity;
	UART_STOPBIT  stop_bit;
	UART_BITDATA bit_data;
	UART_BaudRate  baud_rate;

}UART_ConfigType;





/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(UART_ConfigType * ConfigPtr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
