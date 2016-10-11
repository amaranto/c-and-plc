#include "../inc/main.h"
#include "../inc/buttons.h"
#include "../inc/leds.h"
#include "../inc/uart.h"
#include "../inc/dma.h"
#include "../inc/functions.h"
#include "board.h"
#include "stdint.h"
#include "inttypes.h"

static void initHardware(void);
static void pausems(uint32_t t);

static uint32_t pausems_count;

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 5000);

	/*-------------- Set up buttons and leds hardware ----------------*/
	Buttons_InitAll();

    Chip_SCTPWM_Init(LPC_SCT);
    Chip_SCTPWM_SetRate(LPC_SCT, 10000);

    LEDS_InitCTOUT();

    Chip_SCTPWM_SetOutPin(LPC_SCT, 1, LED2_CTOUT);
    Chip_SCTPWM_SetOutPin(LPC_SCT, 2, LED3_CTOUT);
    Chip_SCTPWM_SetOutPin(LPC_SCT, 3, LED1_CTOUT);

    Chip_SCTPWM_Start(LPC_SCT);

    /*-------------- Set up UART hardware --------------------*/
    UART_Init();

	/*-------------- Set up DMA hardware ---------------------------*/
	//App_DMA_Init();

}

static void pausems(uint32_t t)
{
	pausems_count = t;
	while (pausems_count != 0) {
		__WFI();
	}
}

void SysTick_Handler(void)
{
	if(pausems_count > 0) pausems_count--;
}

int main(void)
{
	char str[MAX_OUTPUT];
	int32_t duty = 0,
			result = 0,
			input_value = 0;
	uint8_t buffer[MAX_BUFFER];

	initHardware();

	sprintf(str,"press button 1 to continue\r\n");
	DEBUGSTR(str);
	while ( Buttons_GetStat_Antishock(BUTTON_1) == NO_BUTTON_PRESSED );

	sprintf(str,"Type : to start \r\n");
	DEBUGSTR(str);

	while (FOREVER)
	{
		LED_Toggle(LEDB);

		/*------------- Reading keyboard ----------*/
		memset ( str, 0, sizeof(str) );
		memset ( buffer, 0 , sizeof( buffer) );
		Chip_UART_Read(LPC_UART, buffer, 1);

		switch ( buffer[0] ) {
			case 13: // Enter
				sprintf ( buffer,"\r\n" );
				break;

			case 58: // :
				memset ( buffer, 0 , sizeof( buffer) ); // clean buffer
				LED_Set (LEDB, 0); // stop led blue and set on led 1 while user inputs data via keyboard
				Chip_SCTPWM_SetDutyCycle(LPC_SCT, 3, Chip_SCTPWM_PercentageToTicks(LPC_SCT, 0xff));

				sprintf(str, "\r\nInsert vale in hex format > ");
				UART_SendBlocking( LPC_UART, str, sizeof(str) );

				UART_ReadBlocking ( LPC_UART, buffer, MAX_BUFFER ); // read the keyboard input
				result = check_input ( buffer );

				if ( result != 256 ) {
					duty = result;
					input_value = result;
				}
				else  DEBUGSTR( " [ Error ] Bad format or out of range " );

				Chip_SCTPWM_SetDutyCycle(LPC_SCT, 3, Chip_SCTPWM_PercentageToTicks(LPC_SCT, 0x00)); // turn off the led 1
				memset ( buffer, 0 , sizeof( buffer) );
				break;

			case 127: // Backspace
				sprintf ( buffer,"\b \b" );
				break;
		}

		UART_SendBlocking(LPC_UART, buffer, sizeof(buffer) );

		/*------------- Checking Buttons ---------*/

		if ( Buttons_GetStat_Antishock(BUTTON_1) != NO_BUTTON_PRESSED ){
			duty = 0x00;
			input_value = 0x00;
			sprintf(str, " Duty level [ %d ]\n\r" , (int)duty);
			UART_SendBlocking( LPC_UART, str, sizeof(str));
		}
		else if ( Buttons_GetStat_Antishock(BUTTON_2) != NO_BUTTON_PRESSED ){
			duty = 0xff;
			input_value = 0xff;
			sprintf(str, " Duty level [ %d ]\n\r" , (int)duty);
			UART_SendBlocking( LPC_UART, str, sizeof(str));
		}

		else if ( Buttons_GetStat_Antishock(BUTTON_3) != NO_BUTTON_PRESSED ){
			duty += 0x10;
			input_value = 0x10;

			if ( duty > 0xff ) duty = 0xff;
			sprintf(str, " Duty level [ %d ]\n\r" , (int)duty);
			UART_SendBlocking( LPC_UART, str, sizeof(str));
		}

		else if ( Buttons_GetStat_Antishock(BUTTON_4) != NO_BUTTON_PRESSED ){
			duty -= 0x10;
			input_value = 0x10;

			if ( duty <= 0 ) duty = 0x00;
			sprintf(str, " Duty level [ %d ]\n\r" , (int)duty);
			UART_SendBlocking( LPC_UART, str, sizeof(str) );
		}
		//App_DMA_Test();
		Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_PercentageToTicks(LPC_SCT, input_value));
		Chip_SCTPWM_SetDutyCycle(LPC_SCT, 2, Chip_SCTPWM_PercentageToTicks(LPC_SCT, duty));
		pausems(DELAY_MS);
	}

	Chip_UART_DeInit(LPC_UART);
}
