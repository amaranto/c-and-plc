#include "../../finaltp/inc/main.h"
#include "../../finaltp/inc/buttons.h"
#include "../../finaltp/inc/leds.h"

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

    Chip_SCU_PinMux(2,10,0,FUNC1);
    /* Chip_SCU_PinMux(2,0,0,FUNC1);
    Chip_SCU_PinMux(2,1,0,FUNC1);
    Chip_SCU_PinMux(2,2,0,FUNC1);

    Chip_SCU_PinMux(2,10,0,FUNC2);
    Chip_SCU_PinMux(2,11,0,FUNC2);
    Chip_SCU_PinMux(2,12,0,FUNC2); */

    Chip_SCTPWM_SetOutPin(LPC_SCT, 1, 2);
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

	sprintf(str, "press button 2 to continue\r\n");
	DEBUGSTR(str);
	while ( Buttons_GetStat(BUTTON_2) == NO_BUTTON_PRESSED );

	sprintf(str, "press button 3 to continue\r\n");
	DEBUGSTR(str);
	while ( Buttons_GetStat(BUTTON_3) == NO_BUTTON_PRESSED );

	sprintf(str, "press button 4 to continue\r\n");
	DEBUGSTR(str);
	while ( Buttons_GetStat(BUTTON_4) == NO_BUTTON_PRESSED );


	while (1)
	{
		Board_LED_Toggle(LED0);

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
			if ( ++led_set > 5 ) led_set = 1;
			sprintf(str, "Working on led %d\r\n" , (int)led_set);
			DEBUGSTR(str);
		}

		else if ( Buttons_GetStat(BUTTON_4) != NO_BUTTON_PRESSED ){
			if ( --led_set < 1 ) led_set = 5;
			sprintf(str, "Working on led %d\r\n" , (int)led_set);
			DEBUGSTR(str);
		}

		Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_PercentageToTicks(LPC_SCT, duty));
		pausems(DELAY_MS);
	}
}
