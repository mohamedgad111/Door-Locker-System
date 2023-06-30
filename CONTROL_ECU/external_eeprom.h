 /******************************************************************************
 *
 * Module: External EEPROM
 *
 * File Name: external_eeprom.h
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/


#ifndef EXTERNAL_EEPROM_H_
#define EXTERNAL_EEPROM_H_

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define ERROR 0
#define SUCCESS 1

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * This function is responsible for writing only one byte of data in the EEPROM in the specified memory address.
 */
uint8 EEPROM_writeByte(uint16 u16addr,uint8 u8data);

/*
 * Description :
 * This function is responsible for reading only one byte of data from the specified memory address in the EEPROM.
 */
uint8 EEPROM_readByte(uint16 u16addr,uint8 *u8data);

#endif /* EXTERNAL_EEPROM_H_ */
