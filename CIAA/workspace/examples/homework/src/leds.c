#include "board.h"
#include "stdint.h"
#include "inttypes.h"
#include "../../finaltp/inc/leds.h"

typedef struct {
	uint8_t port;
	uint8_t pin;
	uint8_t func;
} io_t;

static const io_t scu_rgb[] = { {2,0,0}, {2,1,0} , {2,2,0} };
static const io_t scu_leds[]= { {2,10,0}, {2,11,0}, {2,12,0} };

static const io_t gpio_rgb[]= { {5,0,4}, {5,1,4}, {5,2,4} };
static const io_t gpio_leds[] = { {0,14,0}, {1,11,0}, {1,12,0} };

void Board_LEDS_InitCTOUT( void )
{
   Chip_SCU_PinMux(scu_leds[ LED1 ].port, scu_leds[ LED1 ].pin ,0,FUNC1 );
   Chip_SCU_PinMux(scu_leds[ LED2 ].port, scu_leds[ LED2 ].pin ,0,FUNC1 );
   Chip_SCU_PinMux(scu_leds[ LED3 ].port, scu_leds[ LED3 ].pin, 0,FUNC1 );

}

