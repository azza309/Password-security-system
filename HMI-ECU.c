/**********************************************************************************
 * [FILE NAME]: HMI-ECU.c
 *
 * [AUTHOR]: AZZA SHOKRY
 *
 * [Description]: The main source file for the HMI_ECU
**************************************************************************/

#include "HMI_ECU.h"

int main()
{
	LCD_init();

	UART_init(&UART_configuration);

	LCD_clearScreen();
	LCD_displayStringRowColumn(ROW_ZERO,COLUMN_EIGHT,"DOOR LOCKER");
	LCD_displayStringRowColumn(ROW_ONE,COLUMN_THIRTEEN,"WELCOME ^-^");
	_delay_ms(FOUR_SECONDS_DELAY);

	ifPassSavedORNOT();   /* to check either it's first time for the user to log in or no */

	SREG |= (1<<7);       /* Enable I-Bit for Interrupts */

	while(1){
		(*ptr_states[g_stateNum])();   /* calling functions from the array of functions */
	}

}


void createandStorePassword(void)
{

	if(g_flag==0){
		LCD_clearScreen();
		LCD_displayStringRowColumn(ROW_ZERO,COLUMN_ZERO,"ENTER NEW PASSWORD(5):");
		LCD_goToRowColumn(ROW_ONE,COLUMN_TWELVE);
		fillInPassword(g_password);
		sendCommand(PASSWORD_SEND);
		SendPassword(g_password);
		g_flag=1;
	}
	if(g_flag==1)
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(ROW_ZERO,COLUMN_ZERO,"RE-ENTER PASSWORD(5):");
		LCD_goToRowColumn(ROW_ONE,COLUMN_TWELVE);
		fillInPassword(g_password);
		sendCommand(PASSWORD_CONFIRMATION_SEND);
		SendPassword(g_password);
		g_flag=2;
	}
	if (g_flag==2)
	{
		switch(recieveCommand())
		{
		case PASSWORD_MATCH:
			g_flag=3;
			g_stateNum=1;
			break;

		case PASSWORD_NOT_MATCHED:
			PasswordDONTMATCHScreen();
			g_flag=0;
			break;
		}
	}
}


void SendPassword(uint8 a_arr[])
{
	/*
	 * 1. SEND A READY COMMAND TO THE CONTROL_ECU TO SAY THAT HMI_ECI IS READY
	 * 2.WAITING UNTIL THE CONTROL_ECU SEND A READY COMMAND TOO
	 * 3.SENDING THE PASSWORD ENTERED BY THE USER FROM THE GLOBAL ARRAY
	 * 4. WAITING UNTIL THE CONTROL_ECU SENDS DONE COMMAND
	 */
	UART_sendByte(READY);
	while(UART_recieveByte() != READY){};

	for(uint8 i=0;i<PASSWORD_ARRAY_SIZE;i++)
	{
		UART_sendByte(a_arr[i]);
	}
	while(UART_recieveByte() != DONE){};
}

void sendCommand(uint8 command)
{
	/*
	 * 1. SEND A READY COMMAND TO THE CONTROL_ECU TO SAY THAT HMI_ECI IS READY
	 * 2.WAITING UNTIL THE CONTROL_ECU SEND A READY COMMAND TOO
	 * 3.SENDING A COMMAND FROM THE HMI_ECU
	 * 4. WAITING UNTIL THE CONTROL_ECU SENDS DONE COMMAND
	 */
	UART_sendByte(READY);
	while(UART_recieveByte() != READY){};
	UART_sendByte(command);
	while(UART_recieveByte() != DONE){};

}

uint8 recieveCommand(void)
{
	/*
	 * 1.WAITING UNTIL THE CONTROL_ECU SEND A READY COMMAND
	 * 2. SEND A READY COMMAND TO THE CONTROL_ECU TO SAY THAT HMI_ECI IS READY
	 * 3. RECIEVE A COMMAND FROM THE CONTROL_ECU
	 * 4. SENDING TO THE  CONTROL_ECU  DONE COMMAND
	 * 5. RETURN THE COMMAND SENT FROM THE COTROL_ECU
	 */
	while(UART_recieveByte() != READY){};
	UART_sendByte(READY);
	command=UART_recieveByte();
	UART_sendByte(DONE);
	return command;
}


void mainFucntion(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(ROW_ZERO,COLUMN_SEVEN,"need to CHANGE PASSWORD(+)");
	LCD_displayStringRowColumn(ROW_ONE,COLUMN_SEVEN,"need to OPEN DOOR(-)");
	switch(KeyPad_getPressedKey())
	{
	case '+':
		LCD_clearScreen();
		LCD_displayStringRowColumn(ROW_ZERO,COLUMN_ZERO,"ENTER OLD PASSWORD:(5)");
		LCD_goToRowColumn(ROW_ONE,COLUMN_TWELVE);
		_delay_ms(ONE_AND_HALF_SEC_DELAY);
		fillInPassword(g_password);
		sendCommand(CHECK_PASSWORD);
		SendPassword(g_password);
		switch (recieveCommand())
		{
		case PASSWORD_MATCH:
			g_flag=0;
			g_stateNum=0;
			g_wrong=0;
			break;
		case PASSWORD_NOT_MATCHED:
			wrongPasswordScreen();
			break;
		}
		break;
		case '-':
			LCD_clearScreen();
			LCD_displayStringRowColumn(ROW_ZERO,COLUMN_ZERO,"ENTER OLD PASSWORD:(5)");
			LCD_goToRowColumn(ROW_ONE,COLUMN_TWELVE);
			_delay_ms(ONE_AND_HALF_SEC_DELAY);
			fillInPassword(g_password);
			sendCommand(CHECK_PASSWORD);
			SendPassword(g_password);
			switch (recieveCommand()){
			case PASSWORD_MATCH:
				sendCommand(OPEN_DOOR);
				g_stateNum=2;
				g_wrong=0;
				break;
			case PASSWORD_NOT_MATCHED:
				wrongPasswordScreen();
				break;
			}
			break;
	}

}
/***************************************************************************************************
 * [Function Name]:   ifPassSavedORNOT
 *
 * [Description]:     It checks if it's the user had already entered a password safely and saved
 *                    in the EEPROM or it's his first time creating a password
 *
 * [Arguments]:       VOID
 *
 * [Returns]:         VOID
 ***************************************************************************************************/
void ifPassSavedORNOT(void)
{

	sendCommand(CHECK_IF_SAVED);
	switch (recieveCommand())
	{
	case YES_SAVED:
		g_stateNum=1;
		break;
	case NO_SAVED_PASSWORD:
		g_stateNum=0;
		break;

	}

}


void wrongPasswordScreen(void)
{
	/*
	 *1. DISPLAY A WRONG PASSWORD MESSAGE ON SCREEN
	 *2. INCREMENT THE GLOBAL WRONG VARIABLE
	 *3. SEND COMMANT TO THE CONTROL_ECU THAT THE USER HAS ENTERED A WRONG PASSWORD
	 *4. IF PASSWORD ENTERED IS WRONG 3 TIMES IN A ROW
	 *   IT WILL DISPLAY AN ALARM MESSAGE ON THE SCREEN
	 */
	LCD_clearScreen();
	LCD_displayStringRowColumn(ROW_ZERO,COLUMN_NINE,"WRONG PASSWORD!");
	g_wrong++;
	sendCommand(WRONG_PASSWORD);
	if(g_wrong == MAX_WRONG_COUNTER)
	{
		ALERTscreen();
	}
	_delay_ms(FOUR_SECONDS_DELAY);

}

void fillInPassword(uint8 a_arr[])
{
	uint8 counter=0;
	g_key=KeyPad_getPressedKey();
	while(g_key != '=')
	{
		g_key=KeyPad_getPressedKey();
		if( ( g_key != '+' && g_key != '-' && g_key != '*' && g_key != '%' && g_key != '=') || (g_key>= 0 && g_key<= 9) )
		{
			if(counter < PASSWORD_ARRAY_SIZE)
			{
				a_arr[counter]=g_key;
				LCD_displayCharacter('*');
				counter++;
			}
		}
		_delay_ms(THREE_SECONDS_DELAY);
	}

}



void PasswordDONTMATCHScreen(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(ROW_ZERO,COLUMN_ZERO,"PASSWORD DON'T MATCH !");
	_delay_ms(FOUR_SECONDS_DELAY);

}

void ALERTscreen(void)
{
	/*
	 * 1.INITIATE TIMER1 WITH THE GIVEN CONFIGURATIONS
	 * 2.SET THE CALL BACK FUNCTION
	 * 3. WHILE TIMER HASN'T FINISHED COUNTING DISPLAY ALERT MESSAGE
	 * 4. IF TIME REACHED THE REQUIRED COUNTS
	 *    WRONG COUNTER BACK TO ZERO COUNTS ALSO GO BACK TO THE MAIN FUNCTION
	 */
	LCD_clearScreen();
	Timer_init(&TIMER_configuration);
	Timer_setCallBack(g_tickCounterAlarm,Timer1);

	while(g_tick != TIMER_TICKS_1MINUTE)
	{
		LCD_displayStringRowColumn(ROW_ZERO,COLUMN_TWELVE,"ALERT!!");
		LCD_displayStringRowColumn(ROW_ONE,COLUMN_TWELVE,"THEIF!!");
	}
	if(g_tick==TIMER_TICKS_1MINUTE_STOP)
	{
		g_wrong=0;
		g_stateNum=1;
	}

}

void doorIsOpeningCloseingScreen(void)
{
	/*
	 * 1.INITIATE TIMER1 WITH THE GIVEN CONFIGURATIONS
	 * 2.SET THE CALL BACK FUNCTION
	 * 3. WHILE TIMER HASN'T FINISHED COUNTING DISPLAY DOOR IS UNLOCKING AND LOCKING  MESSAGE
	 * 4. IF TIME REACHED THE REQUIRED COUNTS
	 *    WRONG COUNTER BACK TO ZERO COUNTS ALSO GO BACK TO THE MAIN FUNCTION
	 */
	LCD_clearScreen();
	Timer_init(&TIMER_configuration);
	Timer_setCallBack(g_tickCounterMotor,Timer1);
	while(g_tick != TIMER_TICKS_15SEC)
	{
		LCD_displayStringRowColumn(ROW_ZERO,COLUMN_SEVEN,"Door Opend");
	}
	if(g_tick == TIMER_TICKS_15SEC)
	{
		LCD_clearScreen();
		_delay_ms(FOUR_SECONDS_DELAY);
	}
	while(g_tick>= TIMER_TICKS_15SEC && g_tick < TIMER_TICKS_15SEC_STOP)
	{
		if(g_tick == TIMER_TICKS_15SEC_NEXT_STAGE)
		{
			g_stateNum=1;
		}
		LCD_displayStringRowColumn(0,7,"Door Closed");
	}

}

void g_tickCounterMotor(void)
{
	g_tick++;

	if(g_tick==TIMER_TICKS_15SEC_STOP)
	{
		Timer_DeInit(Timer1);
		g_tick=0;
	}
}

void g_tickCounterAlarm(void)
{
	g_tick++;
	if(g_tick == TIMER_TICKS_1MINUTE_STOP)
	{
		Timer_DeInit(Timer1);
		g_tick=0;

	}
}





