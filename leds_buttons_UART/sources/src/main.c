#include "../inc/main.h"
#include "../inc/buttons.h"
#include "../inc/leds.h"

#include "board.h"
#include "stdint.h"
#include "inttypes.h"

static void initHardware(void);
static void pausems(uint32_t t);

static uint32_t pausems_count;

static void initHardware(void)
{
	Board_Init();
	Board_Buttons_InitAll();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

    Chip_SCTPWM_Init(LPC_SCT);
    Chip_SCTPWM_SetRate(LPC_SCT, 10000);

    Board_LEDS_InitCTOUT();

    Chip_SCTPWM_SetOutPin(LPC_SCT, 1, LED1_CTOUT);
    Chip_SCTPWM_SetOutPin(LPC_SCT, 2, LED2_CTOUT);
    Chip_SCTPWM_SetOutPin(LPC_SCT, 3, LED3_CTOUT);

    Chip_SCTPWM_Start(LPC_SCT);
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
	char str[30];
	int32_t duty = 0;
	int32_t led_set=1;

	initHardware();


	sprintf(str,"press button 1 to continue\r\n");
	DEBUGSTR(str);
	while ( Buttons_GetStat(BUTTON_1) == NO_BUTTON_PRESSED );

	while (1)
	{
		Board_LED_Toggle(LEDG);

		if ( Buttons_GetStat(BUTTON_1) != NO_BUTTON_PRESSED ){
			if ( ++duty    >100 ) duty = 100;
			sprintf(str, "Incrementing duty value to %d\r\n" , (int)duty);
			DEBUGSTR(str);
		}
		else if ( Buttons_GetStat(BUTTON_2) != NO_BUTTON_PRESSED ){
			if ( --duty    <0 ) duty = 0;
			sprintf(str, "Decrementing duty value to %d\r\n" , (int)duty);
			DEBUGSTR(str);
		}

		else if ( Buttons_GetStat(BUTTON_3) != NO_BUTTON_PRESSED ){
			if ( ++led_set > 3 ) led_set = 1;
			sprintf(str, "Working on index %d\r\n" , (int)led_set);
			DEBUGSTR(str);
		}

		else if ( Buttons_GetStat(BUTTON_4) != NO_BUTTON_PRESSED ){
			if ( --led_set < 1 ) led_set = 3;
			sprintf(str, "Working on index %d\r\n" , (int)led_set);
			DEBUGSTR(str);
		}

		Chip_SCTPWM_SetDutyCycle(LPC_SCT, led_set, Chip_SCTPWM_PercentageToTicks(LPC_SCT, duty));
		pausems(DELAY_MS);
	}
}
