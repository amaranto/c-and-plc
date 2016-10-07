#include "../inc/main.h"
#include "../inc/buttons.h"
#include "../inc/leds.h"
#include "../inc/uart.h"
#include "../inc/dma.h"

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
	int32_t duty = 0;
	int32_t input_value = 0;
	uint8_t buffer[MAX_BUFFER];

	initHardware();

	sprintf(str,"press button 1 to continue\r\n");
	DEBUGSTR(str);
	while ( Buttons_GetStat(BUTTON_1) == NO_BUTTON_PRESSED );

	while (FOREVER)
	{
		Board_LED_Toggle(LEDB);

		/*------------- Reading keyboard ----------*/
		memset ( str, 0, sizeof(str) );
		memset ( buffer, 0 , sizeof( buffer) );
		Chip_UART_Read(LPC_UART, buffer, 1);

		switch ( buffer[0] ) {
			case 13: // Enter
				sprintf ( buffer,"\r\n" );
				break;

			case 58:
				memset ( buffer, 0 , sizeof( buffer) );
				Board_LED_Set ( LEDG, 1 );
				Board_LED_Set ( LEDB, 0 );
				UART_ReadBlocking ( LPC_UART, buffer, MAX_BUFFER );
				Board_LED_Set ( LEDG, 0 );
				break;

			case 127: // Backspace
				sprintf ( buffer,"\b \b" );
				break;
		}

		UART_SendBlocking(LPC_UART, buffer, sizeof(buffer) );

		/*------------- Checking Buttons ---------*/

		if ( Buttons_GetStat(BUTTON_1) != NO_BUTTON_PRESSED ){
			duty = 0x00;
			input_value = 0x00;
			sprintf(str, " Duty level [ %d ]\n\r" , (int)duty);
			UART_SendBlocking( LPC_UART, str, sizeof(str));
		}
		else if ( Buttons_GetStat(BUTTON_2) != NO_BUTTON_PRESSED ){
			duty = 0xff;
			input_value = 0xff;
			sprintf(str, " Duty level [ %d ]\n\r" , (int)duty);
			UART_SendBlocking( LPC_UART, str, sizeof(str));
		}

		else if ( Buttons_GetStat(BUTTON_3) != NO_BUTTON_PRESSED ){
			duty += 0x10;
			input_value = 0x10;

			if ( duty > 0xff ) duty = 0xff;
			sprintf(str, " Duty level [ %d ]\n\r" , (int)duty);
			UART_SendBlocking( LPC_UART, str, sizeof(str));
		}

		else if ( Buttons_GetStat(BUTTON_4) != NO_BUTTON_PRESSED ){
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
