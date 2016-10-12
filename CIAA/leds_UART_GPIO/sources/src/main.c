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

	memset ( str, 0, sizeof(str) );
	memset ( buffer, 0, sizeof(str) );

	initHardware();

	sprintf(str,"Press button 1 to continue ... \r\n");
	UART_SendBlocking( LPC_UART, str, sizeof(str) );
	memset ( str, 0, sizeof(str) );

	while ( Buttons_GetStat_Antishock(BUTTON_1) == NO_BUTTON_PRESSED );

	while (FOREVER)
	{
		LED_Toggle(LEDB);

		/*------------- Reading keyboard ----------*/
		memset ( str, 0, sizeof(str) );
		memset ( buffer, 0 , sizeof( buffer) );
		Chip_UART_Read(LPC_UART, buffer, 1);

		switch ( buffer[0] ) {

			case 13: // Enter
				sprintf ( str,"| LED2 [ 0x%X ] | LED3 [ 0x%X ] |\r\n", input_value, duty );
				UART_SendBlocking( LPC_UART, str, sizeof(str) );
				memset ( str, 0, sizeof(str) );
				break;

			case 58: // :
				memset ( buffer, 0 , sizeof( buffer) ); // clean buffer
				LED_Set (LEDB, 0); // stop led blue and set on led 1 while user inputs data via keyboard
				Chip_SCTPWM_SetDutyCycle(LPC_SCT, 3, Chip_SCTPWM_PercentageToTicks(LPC_SCT, 0xff));

				sprintf(str, "\r\n>0x");
				UART_SendBlocking( LPC_UART, str, sizeof(str) );

				UART_ReadBlocking ( LPC_UART, buffer, MAX_BUFFER ); // read the keyboard input
				result = check_input ( buffer );

				if ( result != 256 ) {
					input_value = result;
					duty = interpolate ( result );
					sprintf( str, "Input [ 0x%X ] | Result [ 0x%X ] \r\n", input_value, duty );
					UART_SendBlocking( LPC_UART, str, sizeof(str) );
					memset ( str, 0, sizeof(str) );
				}
				else {
					sprintf( str, "\n\rError ! Value %s out of range.\r\n", buffer );
					UART_SendBlocking( LPC_UART, str, sizeof(str) );
					memset ( str, 0, sizeof(str) );
				}

				Chip_SCTPWM_SetDutyCycle(LPC_SCT, 3, Chip_SCTPWM_PercentageToTicks(LPC_SCT, 0x00)); // turn off the led 1
				memset ( buffer, 0 , sizeof( buffer) );
				break;

			default:
				UART_SendBlocking( LPC_UART, str, sizeof(str) );
				memset ( str, 0, sizeof(str) );
				memset ( buffer, 0 , sizeof( buffer) ); //ignore another input
				break;
		}

		/*------------- Checking Buttons ---------*/

		if ( Buttons_GetStat_Antishock(BUTTON_1) != NO_BUTTON_PRESSED ){
			duty = 0x00;
			input_value = 0x00;
		}
		else if ( Buttons_GetStat_Antishock(BUTTON_2) != NO_BUTTON_PRESSED ){
			duty = 0xff;
			input_value = 0xff;
		}

		else if ( Buttons_GetStat_Antishock(BUTTON_3) != NO_BUTTON_PRESSED ){

			input_value += 0x10;
			if ( input_value > 0xff ) input_value = 0xff;

			duty = interpolate ( input_value );
		}

		else if ( Buttons_GetStat_Antishock(BUTTON_4) != NO_BUTTON_PRESSED ){

			input_value -= 0x10;
			if ( input_value < 0x00 ) input_value = 0x00;
			duty = interpolate ( input_value );
		}
		//App_DMA_Test();
		Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_PercentageToTicks(LPC_SCT, input_value));
		Chip_SCTPWM_SetDutyCycle(LPC_SCT, 2, Chip_SCTPWM_PercentageToTicks(LPC_SCT, duty));
		pausems(DELAY_MS);
	}

	Chip_UART_DeInit(LPC_UART);
}
