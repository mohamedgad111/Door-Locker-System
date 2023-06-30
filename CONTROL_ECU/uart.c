/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author:	Mohamed Gad
 *
 *******************************************************************************/

#include"uart.h"
#include"common_macros.h"/*To use macros like BIT_IS_CLEAR*/
#include<avr/io.h>/*To access UART registers*/


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType* Config_Ptr)
{
	/*
	 * Variable to hold the calculated value responsible for determining the
	 * baud rate; this value is to be assigned to the UBRR register.
	 */
	uint16 ubrr=0;

	/* U2X = 1 for double transmission speed */
	UCSRA = (1<<U2X);

	/*
	 * RXCIE=0 -> receive with interrupt is disabled.
	 * RXEN, TXEN = 1 -> Enable TX and RX pins to work with UARt.
	 * UCSZ2 together with UCSZ1 and UCSZ0 specifies the character size in the UART frame.
	 *
	 */
	UCSRB=(1<<RXEN)|(1<<TXEN)|(GET_BIT(Config_Ptr->bit_data,2)<<UCSZ2);

	/*
	 * URSEL=1 -> write in UCSRC register.
	 * URSEl=0 -> write in UBRR register that controls the baud rate.
	 * UMSEL=1 -> UART works in Asynchronous mode.
	 * UPM0,UPM1 -> selects whether or not to enable parity checker, and if enabled
	 * 	they select between even and odd parity
	 * 	(Note: we only do shift operation with UPM0 because it's the LSB, hence, it adjusts UPM1
	 * 	automatically because it's adjacent to UPM0 in the register.)
	 * USBS -> select the number of the stop bits in the UART frame.
	 * UCSZ1, and UCSZ0 together with UCSZ2 in UCSRB register specifies the character size in the UART frame.
	 */
	UCSRC=(1<<URSEL)|(Config_Ptr->parity<<UPM0)|\
			(Config_Ptr->stop_bit<<USBS)|(GET_BIT(Config_Ptr->bit_data,0)<<UCSZ0)\
			|(GET_BIT(Config_Ptr->bit_data,1)<<UCSZ1);

	/*Calculate the value to be stored in UBRR register to select the desired baud rate*/
	ubrr=(uint16)((float64)(UART_F_CPU)/(float64)(8*Config_Ptr->baud_rate))-1;

	/*
	 * Store the higher 4 bits first, to Clear URSEL bit as well, which indicates we're writing in
	 * UBRR register.
	 *
	 */
	UBRRH=ubrr>>8;

	/*
	 * Then store the least 8 bits in UBRR.
	 * NOOTE: UBRR is a 12-bit register, divided into two 8- bit registers, UBBRL, and UBRRH.
	 */
	UBRRL=ubrr;
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/*
	 * wait until UDR (data register is empty.
	 * UDRE in UCSRA is a flag ,when set indicates that the UDR register is empty.
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE));

	/*assign the data in UDR, and simultaneously start transmission.*/
	UDR=data;
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	/*
	 * Wait until data is received.
	 * RXC in UCSRA is a flag, when set, it indicates that data has been received.
	 */
	while(BIT_IS_CLEAR(UCSRA,RXC));

	/*return the received data, and automatically clear the RXC flag*/
	return UDR;
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	/*variable for string indexing*/
	uint8 str_index;

	/*Send the desired String byte by byte*/
	for(str_index=0;Str[str_index]!='\0';str_index++)
	{
		UART_sendByte(Str[str_index]);
	}
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	/*variable for string indexing*/
	uint8 str_index=0;

	/*
	 * Receive first character to check whether it's '#' or not.
	 * NOTE: '#' indicates that the transmitter is done sending.
	 */
	Str[str_index]=UART_recieveByte();
	while(Str[str_index]!='#')
	{
		/*
		 * Receive the string byte by byte in the Str pointer.
		 * NOTE: Str is sent to the function by reference, meaning that it doesn't
		 * lose the data stored in it.
		 */
		str_index++;
		Str[str_index]=UART_recieveByte();
	}

	/*Replace '#' with the NULL terminator '\0'*/
	Str[str_index]='\0';
}
