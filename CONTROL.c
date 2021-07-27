/**********************************************************************************
 * [FILE NAME]: CONTROL.c
 *
 * [AUTHOR]: AZZA SHOKRY
 *
 * [Description]: The main source file for the CONTROL_ECU
 ***********************************************************************************/

#include"CONTROL_ECU.h"



int main()
{
	UART_init(&UART_configuration);
	SREG |= (1<<7);
		while(1){
		switch(recieveCommand())
		{
		case PASSWORD_SEND:
			recievePassword(g_password);
			break;
		case PASSWORD_CONFIRMATION_SEND:
			recievePassword(g_passmatch);
			if(Match_or_NoMatch(g_password,g_passmatch)){
				storePasswordInMemory();
				getSavedPassword(savedpass);
				sendCommand(PASSWORD_MATCH);
			}
			else
			{
				sendCommand(PASSWORD_NOT_MATCHED);
			}
			break;
		case CHECK_PASSWORD:
			recievePassword(g_password);
			getSavedPassword(savedpass);
			if(Match_or_NoMatch(g_password,savedpass))
			{
				sendCommand(PASSWORD_MATCH);
				g_wrong=0;
			}
			else
			{
				sendCommand(PASSWORD_NOT_MATCHED);
			}
			break;
		case OPEN_DOOR:
			Motor_init();
			Timer_init(&TIMER_configuration);
			Timer_setCallBack(g_tickCounterMotor,Timer1);
			while(g_tick != TICKS_15SECS)
			{
				Motor_ON_ClockWise();
			}
			if(g_tick == TICKS_15SECS)
			{

				Motor_Stop();
				_delay_ms(FOUR_SECONDS_DELAY);
			}
			if(g_tick>= TICKS_15SECS && g_tick<=TICKS_15SECS_REVERSE)
			{
				Motor_ON_Anti_ClockWise();
			}

			break;
		case WRONG_PASSWORD:
			g_wrong++;
			if(g_wrong==MAX_WRONG_COUNTER)
			{
				ALARM_INIT();
				Timer_init(&TIMER_configuration);
				Timer_setCallBack(g_tickCounterAlarm,Timer1);
				while(g_tick != TIMER_TICKS_1MINUTE)
				{
					ALARM_ON();
				}
				if(g_tick == TIMER_TICKS_1MINUTE)
				{
					g_wrong=0;
					ALARM_OFF();
				}
			}
			break;
		case CHECK_IF_SAVED:
			getSavedPassword(savedpass);
			uint8 counter=0;
			for(uint8 i = 0 ; i < PASSWORD_ARRAY_SIZE; i++)
			{
				if(savedpass[i] == DEFUALT_VALUE_OF_EEPROM)
				{
					counter++;
				}

			}
			if (counter==PASSWORD_ARRAY_SIZE)
			{
				sendCommand(NO_SAVED_PASSWORD);
			}
			else
			{
				sendCommand(YES_SAVED);
			}
			break;
		}
	}
}


void recievePassword(uint8 a_arr[])
{
	while(UART_recieveByte() != READY){};
	UART_sendByte(READY);
	for(int i=0 ; i<PASSWORD_ARRAY_SIZE ; i++){
		a_arr[i]=UART_recieveByte();
	}
	UART_sendByte(DONE);
}


bool Match_or_NoMatch(uint8 a_arr1[],uint8 a_arr2[])
{
	uint8 counter=0;
	for(uint8 i=0 ; i<PASSWORD_ARRAY_SIZE ; i++)
	{
		if(a_arr1[i]==a_arr2[i]){
			counter++;
		}
	}
	if(counter==PASSWORD_ARRAY_SIZE){
		return TRUE;
	}
	else{
		return FALSE;
	}
}


void storePasswordInMemory(void)
{

	EEPROM_init();
	for(uint8 i=0;i<PASSWORD_ARRAY_SIZE;i++){
		EEPROM_writeByte(0x0311+i,g_password[i]);
		_delay_ms(HALF_SECOND_DELAY);
	}

}

uint8 recieveCommand(void)
{
	while(UART_recieveByte() != READY){};
	UART_sendByte(READY);
	command=UART_recieveByte();
	UART_sendByte(DONE);
	return command;
}

void sendCommand(uint8 command)
{
	UART_sendByte(READY);
	while(UART_recieveByte() != READY){};
	UART_sendByte(command);
	while(UART_recieveByte() != DONE){};

}

void getSavedPassword(uint8 a_arr[])
{
	EEPROM_init();
	for(uint8 i=0 ; i<PASSWORD_ARRAY_SIZE ; i++){
		EEPROM_readByte(0x0311+i, &a_arr[i]);
		_delay_ms(1000);
	}
}


void g_tickCounterMotor(void)
{
	g_tick++;
	if(g_tick==TIMER_TICKS_15SEC_STOP)
	{
		Timer_DeInit(Timer1);
		Motor_Stop();
		g_tick=0;
	}
}

void g_tickCounterAlarm(void)
{
	g_tick++;
	if(g_tick==TIMER_TICKS_1MINUTE_STOP)
	{
		Timer_DeInit(Timer1);
		g_tick=0;

	}
}
