 /******************************************************************************
 *
 * Module: Control ECU
 *
 * File Name: CONTROL_ECU.h
 *
 * Author: Mohamed Gad
 *
 *******************************************************************************/

#ifndef CONTROL_ECU_H_
#define CONTROL_ECU_H_

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define F_CPU 8000000UL

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum{
	PASSWORD_FAILED,PASSWORD_PASSED,THIEF
}Password_Status;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * This function receives the two passwords entered by the user from HMI_ECU, and
 * compare them to see whether they match or not.
 */
Password_Status receiveAndCheckPassword(uint8*pass_one,uint8*pass_two);

/*
 * Description :
 * This function saves the validated password in the external EEPROM.
 */
void savePassword(const uint8*pass);

/*
 * Description :
 * This function reads the saved password from the external EEPROM.
 */
void retrievePassword(uint8* pass);

/*
 * Description :
 * This function compares the input password with the one save in the EEPROM,
 * and return whether the password is correct or not.
 */
Password_Status confirmPassword(uint8*pass_one);

/*
 * Description :
 * This is the callBack Function that'll be called when an interrupt request is generated
 * by Timer1.
 */
void callBack(void);

#endif /* CONTROL_ECU_H_ */
