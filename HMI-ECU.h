/**********************************************************************************
 * [FILE NAME]: HMI-ECU.h
 *
 * [AUTHOR]: AZZA SHOKRY
 *
 * [Description]: The main header file for the HMI_ECU
 * contents:
 *          DRIVERS USED IN THE HMI_ECU
 *          DEFINED COMMANDS
 *          UART & TIMER CONFIGURATIONS
 *          GLOBALE VARIABLES
 *          FUNCTIONS
 *          ARRAY OF POINTER TO FUNCTIONS THAT TAKE VOID AS AN ARGUMENT AND RETURN VOID
 ***********************************************************************************/

#ifndef HMI_ECU_H_
#define HMI_ECU_H_

/****HMI_ECU**/


/****DRIVERS INCLUDES****/
#include"LCD.h"
#include"keypad.h"
#include"UART.h"
#include"common_macros.h"
#include"std_types.h"
#include"micro_config.h"
#include"TIMER.h"

/**DEFINED COMMANDS**/

#define READY                                               0x01
#define DONE                                                0x02
#define PASSWORD_SEND                                       0x03
#define PASSWORD_MATCH                                      0x04
#define PASSWORD_NOT_MATCHED                                0x05
#define PASSWORD_CONFIRMATION_SEND                          0x06
#define CHANGE_PASSWORD                                     0x07
#define OPEN_DOOR                                           0x08
#define CHECK_PASSWORD                                      0x09
#define WRONG_PASSWORD                                      0xA
#define CHECK_IF_SAVED                                      0xB
#define YES_SAVED                                           0xC
#define NO_SAVED_PASSWORD                                   0xD
#define PASSWORD_ARRAY_SIZE                                 5
#define ONE_AND_HALF_SEC_DELAY                              1500
#define THREE_SECONDS_DELAY                                 3000
#define FOUR_SECONDS_DELAY                                  4000
#define ROW_ZERO                                            0
#define ROW_ONE                                             1
#define COLUMN_ZERO                                         0
#define COLUMN_SEVEN                                        7
#define COLUMN_EIGHT                                        8
#define COLUMN_NINE                                         9
#define COLUMN_TWELVE                                       12
#define COLUMN_THIRTEEN                                     13
#define TIMER_TICKS_15SEC                                   2
#define TIMER_TICKS_15SEC_STOP                              4
#define TIMER_TICKS_15SEC_NEXT_STAGE                        3
#define TIMER_TICKS_1MINUTE                                 12
#define TIMER_TICKS_1MINUTE_STOP                            13
#define GLOBALE_ARRAY_OF_POINTERS_SIZE                      3
#define MAX_WRONG_COUNTER                                   3
/***UART & TIMER CONFIGURATIONS*******/
/* ASYNCHRONOUS: UART_mode
 * DISABLED: UART_parityBit
 * ONE : UART_stopBit
 * EIGHT : UART_characterSize
 */
UART_config UART_configuration = {ASYNCHRONOUS,DISABLED,ONE,EIGHT};

/* 0: timer_InitialValue
 * 39062: timer_compare_MatchValue
 * Timer1 : timer_ID
 * F_CPU_1024 : timer_clock
 * Compare : timer_mode
 */

Timer_Config TIMER_configuration= {0,39062,Timer1,F_CPU_1024,Compare};

/******GLOBAL VARIABLES***********/
static volatile uint8 g_flag=0;               /*global flag variable to check on */
static volatile uint8  g_stateNum;            /*global state number of the main function */
uint8 g_key;                                  /*global variable to hold the key value */
uint8 g_password[5];                          /*global array to store the password in it */
uint8 command;                                /*global variable to store the commands in it */
uint8 g_wrong=0;                              /*global variable to count wrong password entered times*/
uint8 g_tick=0;                               /*global ticks to count timer interrupts*/


void createandStorePassword(void);



void SendPassword(uint8 a_arr[]);



void sendCommand(uint8 command);


uint8 recieveCommand(void);


void mainFucntion(void);


void ifPassSavedORNOT(void);


void wrongPasswordScreen(void);


void fillInPassword(uint8 a_arr[]);


void PasswordDONTMATCHScreen(void);

void ALERTscreen(void);


void doorIsOpeningCloseingScreen(void);



void g_tickCounterMotor(void);


void g_tickCounterAlarm(void);




void (*ptr_states[GLOBALE_ARRAY_OF_POINTERS_SIZE])(void)={createandStorePassword,mainFucntion,doorIsOpeningCloseingScreen};


#endif /* HMI_ECU_H_ */
