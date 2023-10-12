/*
 * main.c
 *
 *  Created on: Jul 7, 2020
 *      Author: khaled
 */

#include "../Lib/Std_types.h"
#include "../Lib/Bit_math.h"

#include "Uart_int.h"
#include "I2c_int.h"
#include "Eeprom_int.h"

#undef F_CPU
#define F_CPU 8000000
#include <util/delay.h>


#define PASSWORD_ARR_LENGTH      4
#define PASSWORDCONF_ARR_LENGTH  4

#define PASSWORD_FLAG_LOC      4
#define SYSTEM_FLAG_LOC        5

#define PASSWORD_EXIST    1+0x30
#define SYSTEM_NOTBLOCKED 0+0x30
#define SYSTEM_BLOCKED    1+0x30


u8 Password[4];
u8 Password_Confirm[4];

int main(void)
{
	
	/* flag to check password existence in Eeprom MEM  */
	u8 Password_flag;
	/* flag to check system availability */
	u8 System_flag ;
	/* iterator to loop in PW arrays */
	u8 i ;

	/* Initialize UART module */
	Uart_vidInit();
	/* Initialize EEPROM module */
	Eeprom_vidInit();

	/* display welcome MSG sent from UART on PC terminal */
	Uart_vidSendString("Welcome to PassWord updater\r\n");


	while(1)
	{

		/* check password existence flag (read it from EEPROM) */
		Eeprom_u8ReadByte(EEPROM_u8DEVICE_0 , PASSWORD_FLAG_LOC, &Password_flag);

		/* if there is no password saved in EEPROM (the flag is cleared) */
		if(Password_flag != PASSWORD_EXIST)
		{
			/* Label */
			NEW_PW :
			/* display PW entering MSG sent from UART on PC terminal */
			Uart_vidSendString("Enter new password\r\n");

			/* UART receive PW from the user through PC terminal  */
			for(i=0 ; i < PASSWORD_ARR_LENGTH ; i++)
			{
				/* save the entered password */
				Password[i] = Uart_vidReceiveByte();
				/* display what the user entered */
				Uart_vidSendByte(Password[i]);
			}
			Uart_vidSendByte('\r');
			Uart_vidSendByte('\n');

			/* display PW confirmation MSG sent from UART on PC terminal */
			Uart_vidSendString("Please enter the password again\r\n");

			/* UART receive confirm PW from the user through PC terminal  */
			for(i=0 ; i < PASSWORDCONF_ARR_LENGTH ; i++)
			{
				/* save the entered password */
				Password_Confirm[i] = Uart_vidReceiveByte();
				/* display what the user entered */
				Uart_vidSendByte(Password_Confirm[i]);
			}
			Uart_vidSendByte('\r');
			Uart_vidSendByte('\n');

			

			/*compare 2 passwords*/
			/* if 2 passwords match */
			if((Password[0]==Password_Confirm[0])&&(Password[1]==Password_Confirm[1])&&(Password[2]==Password_Confirm[2])&&(Password[3]==Password_Confirm[3]))
			{
				/* save the password in EEPROM */
				for(i=0 ; i < PASSWORD_ARR_LENGTH ; i++)
				{
					Eeprom_u8WriteByte(EEPROM_u8DEVICE_0, i, Password[i]);
					_delay_ms(10);
				}
				/* set the flag of password validity in EEPROM */
				Eeprom_u8WriteByte(EEPROM_u8DEVICE_0, PASSWORD_FLAG_LOC, PASSWORD_EXIST);
				_delay_ms(10);
				/* clear the flag of system block in EEPROM */
				Eeprom_u8WriteByte(EEPROM_u8DEVICE_0, SYSTEM_FLAG_LOC, SYSTEM_NOTBLOCKED);
				_delay_ms(10);
				Uart_vidSendString("Thank you the Password is saved\r\n");
				break;
			}

			/* if 2 passwords do not match */
			else
			{
				/* display 2 passwords not correct MSG sent from UART on PC terminal */
				Uart_vidSendString("two passwords do not match !Try Again! \r\n");
				_delay_ms(500);
				continue ;
			}
		}

		/* if there is a password saved in EEPROM (the flag is set) */
		else
		{
			/* display update password MSG sent from UART on PC terminal */
			Uart_vidSendString("Modify the Password saved:\r\n");
			_delay_ms(500);

			/* display enter the old password MSG sent from UART on PC terminal */
			Uart_vidSendString("Please enter the old Password \r\n");

			/* UART receive PW from the user through PC terminal  */
			for(i=0 ; i < PASSWORDCONF_ARR_LENGTH ; i++)
			{
				Password_Confirm[i] = Uart_vidReceiveByte();
				/* display what the user entered */
				Uart_vidSendByte(Password_Confirm[i]);
			}
			Uart_vidSendByte('\r');
			Uart_vidSendByte('\n');
			
			/* Read the saved password from EEPROM */
			for(i=0 ; i < PASSWORD_ARR_LENGTH ; i++)
			{
				Eeprom_u8ReadByte(EEPROM_u8DEVICE_0 , i, &Password[i]);
				_delay_ms(10);
			}
			/*compare 2 passwords*/
			/* if old password is correct */
			if((Password[0]==Password_Confirm[0])&&(Password[1]==Password_Confirm[1])&&(Password[2]==Password_Confirm[2])&&(Password[3]==Password_Confirm[3]))
			{
				goto NEW_PW ;
			}
			/* if old password is not correct */
			else
			{
				/* display 2 passwords not correct MSG sent from UART on PC terminal */
				Uart_vidSendString("the old Password is not correct !Try Again! \r\n");
				_delay_ms(500);
				continue ;
			}

		}
	}
	
	return 0 ;
}
