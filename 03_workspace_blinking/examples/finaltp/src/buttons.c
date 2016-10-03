#include "board.h"
#include "stdint.h"
#include "inttypes.h"
#include "../../finaltp/inc/buttons.h"

typedef	struct {
	uint8_t port;
	uint8_t pin;
	uint8_t func;
} io_t;

static const io_t gpioButtons[] = { {0,4,0},{0,8,0},{0,9,0},{1,9,0} };
static const io_t scuButtons[]  = { {1,0,0},{1,1,0},{1,2,0},{1,6,0} };

void Board_Buttons_InitAll(void)
{
	int i=0;
	for ( i=0; i < TOTAL_BUTTONS; i++) {
		Chip_SCU_PinMux( scuButtons[i].port , scuButtons[i].pin ,MD_PUP|MD_EZI|MD_ZI, scuButtons[i].func ); /* GPIO0[4], SW1 */
		Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpioButtons[i].port, gpioButtons[i].pin );
	}
}

uint32_t Buttons_GetStat( int32_t button )
{
	uint8_t gpio_port = 0;
	uint8_t gpio_bit = 0;
	uint8_t ret = NO_BUTTON_PRESSED;
	char str[30];

	if ( button < 0 || TOTAL_BUTTONS < button ){
		sprintf(str, "[ Button ] Value %d is out of range ! ", (int)button );
		DEBUGSTR(str);
		return ret;
	}

	gpio_port = gpioButtons[ button ].port;
	gpio_bit = gpioButtons[ button ].pin;

	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, gpio_port, gpio_bit) == 0) {
		ret |= 0X01;
	}

	//sprintf(str, "[ Button ] Number %d return value %d ", (int)button, (int)ret );
	//DEBUGSTR(str);
	return ret;
}
