/******************************************************************************
 *
 * Module: TWI Module (I2C)
 *
 * File Name: twi.c
 *
 * Description: Source file for TWI Module
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "twi.h"
#include "common_macros.h"
#include <avr/io.h>

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 *	1- This Function initializes the TWI Module with the specified configurations
 *	   related to slave address, and Bit Rate that are sent to the function in the structure
 *	   TWI_ConfigType.
 */
void TWI_init(const TWI_ConfigType * Config_Ptr)
{
	/*Assign the TWBR value that corresponds to the required bit-rate in the configuration structure*/
	switch (Config_Ptr->bit_rate)
	{
	case NORMAL_MODE:
		if(TWI_F_CPU==8000000UL)
		{
			TWBR=32;

			/*Using zero pre-scaler TWPS=00*/
			TWSR=0;
		}
		else if(TWI_F_CPU==16000000UL)
		{
			TWBR=72;

			/*Using zero pre-scaler TWPS=00*/
			TWSR=0;
		}
		break;
	case FAST_MODE:
		if(TWI_F_CPU==8000000UL)
		{
			TWBR=2;

			/*Using zero pre-scaler TWPS=00*/
			TWSR=0;
		}
		else if(TWI_F_CPU==16000000UL)
		{
			TWBR=12;

			/*Using zero pre-scaler TWPS=00*/
			TWSR=0;
		}
		break;
	case FAST_MODE_PLUS:
		/*
		 * Operate with slow bit-rate (100kb/s) because this bit-rate cannot be
		 * generated when the F_CPU is either 8MHz or 16MHz.
		 */
		TWBR=32;

		/*Using zero pre-scaler TWPS=00*/
		TWSR=0;
		break;
	case HIGH_SPEED_MODE:
		/*
		 * Operate with slow bit-rate (100kb/s) because this bit-rate cannot be
		 * generated when the F_CPU is either 8MHz or 16MHz.
		 */
		TWBR=32;

		/*Using zero pre-scaler TWPS=00*/
		TWSR=0;
		break;
	}

	/*
	 * Assign the device address in TWAR when this device is slave:
	 * 1) Turn off General call recognition (bit-0), TWGCE=0
	 * 2) Write the device address in bits (TWA0 to TWA6).
	 */
	TWAR=(Config_Ptr->address<<TWA0);

	/*Enable TWI*/
	TWCR = (1<<TWEN);
}

/*
 * Description :
 * This Function sends the start bit on the data bus of the TWI.
 */
void TWI_start(void)
{
	/*
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * send the start bit by TWSTA=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	/*
	 * Wait for TWINT flag set in TWCR Register,
	 * before sending any further data (wait until start bit is sent successfully)
	 */
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

/*
 * Description :
 * This Function sends the stop bit on the data bus of the TWI.
 */
void TWI_stop(void)
{
	/*
	 * Clear the TWINT flag before sending the stop bit TWINT=1
	 * send the stop bit by TWSTO=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

/*
 * Description :
 * This Function sends only one byte of data on the data bus of the TWI.
 */
void TWI_writeByte(uint8 data)
{
	/* Put data On TWI data Register */
	TWDR = data;

	/*
	 * Clear the TWINT flag before sending the data TWINT=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWEN);

	/* Wait for TWINT flag set in TWCR Register(data is send successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

/*
 * Description :
 * This Function reads only one byte of data and raises an acknowledgment that indicates the success of transmission
 * and the readiness for more data to be received.
 */
uint8 TWI_readByteWithACK(void)
{
	/*
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable sending ACK after reading or receiving data TWEA=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	/* Wait for TWINT flag set in TWCR Register (data received successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));

	/* Read Data */
	return TWDR;
}

/*
 * Description :
 * This Function reads only one byte of data and raises a negative acknowledgment that indicates the success of transmission
 * and the desire to stop sending more data.
 */
uint8 TWI_readByteWithNACK(void)
{
	/*
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1 << TWINT) | (1 << TWEN);

	/* Wait for TWINT flag set in TWCR Register (data received successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));

	/* Read Data */
	return TWDR;
}

/*
 * Description :
 * This Function returns the status of the TWI logic and the Two-Wire Serial bus.
 */
uint8 TWI_getStatus(void)
{
	uint8 status=0;

	/*Masking to eliminate first 3 bits and get the last 5 bits (status bits) */
	status= TWSR&0xF8;

	return status;
}
