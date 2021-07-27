/**********************************************************************************
 * [FILE NAME]: CONTROL.h
 *
 * [AUTHOR]: AZZA SHOKRY
 *
 * [Description]: The main header file for the CONTROL_ECU
 * contents:
 *
 ***********************************************************************************/

#ifndef CONTROL_ECU_H_
#define CONTROL_ECU_H_

/***CONTROL_ECU****/


/** DRIVERS INCLUDES**************************/

#include"external_eeprom.h"
#include"micro_config.h"
#include"std_types.h"
#include"UART.h"
#include"common_macros.h"
#include"Motor.h"
#include"TIMER.h"
#include"ALARM.h"


/********DEFINED COMMANDS***/

#define READY                                              0x01
#define DONE                                               0x02
#define PASSWORD_SEND                                      0x03
#define PASSWORD_MATCH                                     0x04
#define PASSWORD_NOT_MATCHED                               0x05
#define PASSWORD_CONFIRMATION_SEND                         0x06
#define CHANGE_PASSWORD                                    0x07
#define OPEN_DOOR                                          0x08
#define CHECK_PASSWORD                                     0x09
#define WRONG_PASSWORD                                      0xA
#define CHECK_IF_SAVED                                      0xB
#define YES_SAVED                                           0xC
#define NO_SAVED_PASSWORD                                   0xD
#define TICKS_15SECS                                        2
#define TICKS_15SECS_REVERSE                                3
#define TIMER_TICKS_15SEC_STOP                              4
#define TIMER_TICKS_1MINUTE                                 12
#define TIMER_TICKS_1MINUTE_STOP                            13
#define FOUR_SECONDS_DELAY                                  4000
#define HALF_SECOND_DELAY                                   500
#define MAX_WRONG_COUNTER                                   3
#define PASSWORD_ARRAY_SIZE                                 5
#define DEFUALT_VALUE_OF_EEPROM                             1

/***************UART & TIMER CONFIGURATIONS*****/

UART_config UART_configuration = {ASYNCHRONOUS,DISABLED,ONE,EIGHT};
Timer_Config TIMER_configuration= {0,39062,Timer1,F_CPU_1024,Compare};


/****GLOBAL VARIABLES***/

uint8 g_password[5];
uint8 g_passmatch[5];
uint8 savedpass[5];
uint8 command;
uint8 g_tick=0;
uint8 g_wrong=0;

/**FUNCTIONS*******/
void recievePassword(uint8 a_arr[]);


void storePasswordInMemory(void);


bool Match_or_NoMatch(uint8 a_arr1[],uint8 a_arr2[]);


uint8 recieveCommand(void);


void sendCommand(uint8 command);


void getSavedPassword(uint8 a_arr[]);



void g_tickCounterMotor(void);


void g_tickCounterAlarm(void);

#endif /* CONTROL_ECU_H_ */
