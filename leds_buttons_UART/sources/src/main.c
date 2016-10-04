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

    Chip_SCTPWM_SetOutPin(LPC_SCT, 1, LED2_CTOUT);
    Chip_SCTPWM_SetOutPin(LPC_SCT, 2, LED3_CTOUT);

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
	int32_t input_value = 0;
	int32_t led_set=1;

	initHardware();


	sprintf(str,"press button 1 to continue\r\n");
	DEBUGSTR(str);
	while ( Buttons_GetStat(BUTTON_1) == NO_BUTTON_PRESSED );

	while (1)
	{
		Board_LED_Toggle(LEDG);

		if ( Buttons_GetStat(BUTTON_1) != NO_BUTTON_PRESSED ){
			duty = 0x00;
			input_value = 0x00;
			sprintf(str, "Set value to %d\r\n" , (int)duty);
			DEBUGSTR(str);
		}
		else if ( Buttons_GetStat(BUTTON_2) != NO_BUTTON_PRESSED ){
			duty = 0xff;
			input_value = 0xff;
			sprintf(str, "Set value to %d\r\n" , (int)duty);
			DEBUGSTR(str);
		}

		else if ( Buttons_GetStat(BUTTON_3) != NO_BUTTON_PRESSED ){
			duty += 0x10;
			input_value = 0x10;

			if ( duty > 0xff ) duty = 0xff;
			sprintf(str, "Duty level [ %d ]\r\n" , (int)duty);
			DEBUGSTR(str);
		}

		else if ( Buttons_GetStat(BUTTON_4) != NO_BUTTON_PRESSED ){
			duty -= 0x10;
			input_value = 0x10;

			if ( duty <= 0 ) duty = 0x00;
			sprintf(str, "Duty level [ %d ]\r\n" , (int)duty);
			DEBUGSTR(str);
		}

		Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_PercentageToTicks(LPC_SCT, input_value));
		Chip_SCTPWM_SetDutyCycle(LPC_SCT, 2, Chip_SCTPWM_PercentageToTicks(LPC_SCT, duty));
		pausems(DELAY_MS);
	}
}
