#include "board.h"
#include "stdint.h"
#include "inttypes.h"
#include "string.h"
#include "../inc/leds.h"


typedef struct {
	uint8_t port;
	uint8_t pin;
	uint8_t func;
} io_t;

static const io_t scu_leds[]= { {2,10,0}, {2,11,0}, {2,12,0} ,{2,0,0}, {2,1,0} , {2,2,0}  };
static const io_t gpio_leds[] = { {0,14,0}, {1,11,0}, {1,12,0} , {5,0,4}, {5,1,4}, {5,2,4} };

void LEDS_InitCTOUT( void )
{
   Chip_SCU_PinMux(scu_leds[ LED1 ].port, scu_leds[ LED1 ].pin ,0,FUNC1 );
   Chip_SCU_PinMux(scu_leds[ LED2 ].port, scu_leds[ LED2 ].pin ,0,FUNC1 );
   Chip_SCU_PinMux(scu_leds[ LED3 ].port, scu_leds[ LED3 ].pin, 0,FUNC1 );

}

void LED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber < ( sizeof(gpio_leds)/sizeof(io_t) ) )
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, gpio_leds[LEDNumber].port, gpio_leds[LEDNumber].pin, On);
}

bool LED_Test(uint8_t LEDNumber)
{
	if (LEDNumber < ( sizeof(gpio_leds)/sizeof(io_t) ) )
		return (bool) Chip_GPIO_GetPinState(LPC_GPIO_PORT, gpio_leds[LEDNumber].port, gpio_leds[LEDNumber].pin );

	return false;
}

void LED_Toggle(uint8_t LEDNumber)
{
	LED_Set(LEDNumber, !LED_Test(LEDNumber));
}
