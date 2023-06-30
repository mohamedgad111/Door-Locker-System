/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author:	Mohamed Gad
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/*CPU Clock Frequency, used in calculating The baud rate of the UART to be stored in UBRR register.*/
#define UART_F_CPU 8000000UL

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

/*Enum responsible for enabling or disabling the parity checker*/
typedef enum{
	PARITY_DISABLED,EVEN_PARITY=2,ODD_PRITY
}UART_Parity;

/*Enum responsible for specifying how many stop bits in the UART frame*/
typedef enum{
	ONE_STOP,TWO_STOP
}UART_StopBit;

/*Enum responsible for specifying the number of data bits in the UARt frame*/
typedef enum{
	FIVE_BITS,SIX_BITS,SEVEN_BITS,EIGHT_BITS,NINE_BITS=7
}UART_BitData;

typedef uint32 UART_BaudRate;

/*Configuration Structure of the UART module, used in to be sent to the UART_init() function*/
typedef struct{
	UART_Parity parity;
	UART_StopBit stop_bit;
	UART_BitData bit_data;
	UART_BaudRate baud_rate;
}UART_ConfigType;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType* Config_Ptr);

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
void UART_receiveString(uint8 *Str); /* Receive until #*/

#endif /* UART_H_ */
