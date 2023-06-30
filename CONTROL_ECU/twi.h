/******************************************************************************
 *
 * Module: TWI(I2C)
 *
 * File Name: twi.h
 *
 * Description: Header file for the TWI(I2C) AVR driver
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

#ifndef TWI_H_
#define TWI_H_

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define TWI_START         0x08 /* start has been sent */
#define TWI_REP_START     0x10 /* repeated start */
#define TWI_MT_SLA_W_ACK  0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define TWI_MT_SLA_R_ACK  0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define TWI_MT_DATA_ACK   0x28 /* Master transmit data and ACK has been received from Slave. */
#define TWI_MR_DATA_ACK   0x50 /* Master received data and send ACK to slave. */
#define TWI_MR_DATA_NACK  0x58 /* Master received data but doesn't send ACK to slave. */

/*CPU_Frequency*/
#define TWI_F_CPU		  8000000UL
#define TWI_1MHZ		  1000000UL

/*******************************************************************************
 *                      Preprocessor Error                                    *
 *******************************************************************************/
#if (TWI_F_CPU == TWI_1MHZ)

#error "Operational CPU cannot drive the TWI interface, change it to be at least 8MHz."

#endif

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef uint8 TWI_Address;

typedef enum{
	NORMAL_MODE=100000,FAST_MODE=400000,FAST_MODE_PLUS=1000000,HIGH_SPEED_MODE=3400000
}TWI_BaudRate;

typedef struct{
	TWI_Address address;
	TWI_BaudRate bit_rate;
}TWI_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * This Function initializes the TWI Module with the specified configurations
 * related to slave address, and Bit Rate that are sent to the function in the structure
 * TWI_ConfigType.
 */
void TWI_init(const TWI_ConfigType * Config_Ptr);

/*
 * Description :
 * This Function sends the start bit on the data bus of the TWI.
 */
void TWI_start(void);

/*
 * Description :
 * This Function sends the stop bit on the data bus of the TWI.
 */
void TWI_stop(void);

/*
 * Description :
 * This Function sends only one byte of data on the data bus of the TWI.
 */
void TWI_writeByte(uint8 data);

/*
 * Description :
 * This Function reads only one byte of data and raises an acknowledgment that indicates the success of transmission
 * and the readiness for more data to be received.
 */
uint8 TWI_readByteWithACK(void);

/*
 * Description :
 * This Function reads only one byte of data and raises a negative acknowledgment that indicates the success of transmission
 * and the desire to stop sending more data.
 */
uint8 TWI_readByteWithNACK(void);

/*
 * Description :
 * This Function returns the status of the TWI logic and the Two-Wire Serial bus.
 */
uint8 TWI_getStatus(void);


#endif /* TWI_H_ */
