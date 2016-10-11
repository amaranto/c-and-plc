#include "board.h"
#include "stdint.h"
#include "inttypes.h"
#include "../inc/buttons.h"


typedef	struct {
	uint8_t port;
	uint8_t pin;
	uint8_t func;
} io_t;

static const io_t gpioButtons[] = { {0,4,0},{0,8,0},{0,9,0},{1,9,0} };
static const io_t scuButtons[]  = { {1,0,0},{1,1,0},{1,2,0},{1,6,0} };


void Buttons_InitAll(void)
{
	int i=0;
	for ( i=0; i < TOTAL_BUTTONS; i++) {
		Chip_SCU_PinMux( scuButtons[i].port , scuButtons[i].pin ,MD_PUP|MD_EZI|MD_ZI, scuButtons[i].func ); /* GPIO0[4], SW1 */
		Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpioButtons[i].port, gpioButtons[i].pin );
	}
}

uint32_t Buttons_GetStat( int32_t button )
{
	uint8_t ret = NO_BUTTON_PRESSED;
	char str[30];

	if ( button < 0 || TOTAL_BUTTONS < button ){
		sprintf(str, "[ Button ] Value %d is out of range ! ", (int)button );
		DEBUGSTR(str);
		return ret;
	}

	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, gpioButtons[ button ].port, gpioButtons[ button ].pin ) == 0) {
		ret |= 0X01;
	}

	return ret;
}

uint32_t Buttons_GetStat_Antishock( int32_t button )
{
	uint8_t i = 0,
			ret=NO_BUTTON_PRESSED;
	while ( Buttons_GetStat(button) != NO_BUTTON_PRESSED ) i++;

	Chip_GPIO_SetPinState (LPC_GPIO_PORT, gpioButtons[ button ].port, gpioButtons[ button ].pin, 1 );

	if ( i < ANTISHOCK ) return NO_BUTTON_PRESSED;
	else return 0x10;
}
