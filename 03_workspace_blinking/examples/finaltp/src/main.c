/* Copyright 2016, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
/** @brief This is a simple blink example.
 */

/** \addtogroup blink Bare-metal blink example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "../../finaltp/inc/main.h"

#include "board.h"
#include "stdint.h"
#include "inttypes.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/** @brief delay function
 * @param t desired milliseconds to wait
 */
static void pausems(uint32_t t);

/*==================[internal data definition]===============================*/

/** @brief used for delay counter */
static uint32_t pausems_count;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	Board_Buttons_InitAll();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}

static void Board_Buttons_InitAll(void)
{
	Chip_SCU_PinMux(1,0,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[4], SW1 */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM);	// input
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM);
	// No funciona

	Chip_SCU_PinMux(1,1,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[4], SW1 */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM);	// input

	Chip_SCU_PinMux(1,2,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[4], SW1 */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON3_GPIO_PORT_NUM, BUTTONS_BUTTON3_GPIO_BIT_NUM);	// input

	Chip_SCU_PinMux(1,6,MD_PUP|MD_EZI|MD_ZI,FUNC0); /* GPIO0[4], SW1 */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON4_GPIO_PORT_NUM, BUTTONS_BUTTON4_GPIO_BIT_NUM);	// input

}

uint32_t Buttons_GetStat( int32_t button )
{
	uint8_t tmp_gpio_port = 0;
	uint8_t tmp_gpio_bit = 0;
	uint8_t ret = NO_BUTTON_PRESSED;
	char str [30];

	switch ( button ) {

		case 1:
			tmp_gpio_port = BUTTONS_BUTTON1_GPIO_PORT_NUM;
			tmp_gpio_bit  = BUTTONS_BUTTON1_GPIO_BIT_NUM;
			break;

		case 2:
			tmp_gpio_port = BUTTONS_BUTTON2_GPIO_PORT_NUM;
			tmp_gpio_bit  = BUTTONS_BUTTON2_GPIO_BIT_NUM;
			break;

		case 3:
			tmp_gpio_port = BUTTONS_BUTTON3_GPIO_PORT_NUM;
			tmp_gpio_bit  = BUTTONS_BUTTON3_GPIO_BIT_NUM;
			break;

		case 4:
			tmp_gpio_port = BUTTONS_BUTTON4_GPIO_PORT_NUM;
			tmp_gpio_bit  = BUTTONS_BUTTON4_GPIO_BIT_NUM;
			break;
	}


	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, tmp_gpio_port, tmp_gpio_bit) == 0) {
		ret |= BUTTONS_BUTTON1;
	}
	return ret;
}

static void pausems(uint32_t t)
{
	pausems_count = t;
	while (pausems_count != 0) {
		__WFI();
	}
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
	if(pausems_count > 0) pausems_count--;
}

int main(void)
{
	char str[30];
	int32_t cont = 0;
	int32_t btn=0;

	initHardware();


	sprintf(str,"press button 1 to continue");
	DEBUGSTR(str);

	while ( Buttons_GetStat(1) == NO_BUTTON_PRESSED );
	sprintf(str,"button pressed");
	DEBUGSTR(str);

	// Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0,(1<<4));
	// Chip_GPIO_SetValue (LPC_GPIO_PORT, BUTTONS_BUTTON2_GPIO_PORT_NUM, 1);
	sprintf(str, "press button 2 to continue");
	DEBUGSTR(str);
	while ( Buttons_GetStat(2) == NO_BUTTON_PRESSED );

	sprintf(str, "press button 3 to continue");
	DEBUGSTR(str);
	while ( Buttons_GetStat(3) == NO_BUTTON_PRESSED );

	sprintf(str, "press button 4 to continue");
	DEBUGSTR(str);
	while ( Buttons_GetStat(4) == NO_BUTTON_PRESSED );

	btn = Buttons_GetStat(4);
	sprintf(str,"tmp = %"PRIu32"\r\n", btn);
	DEBUGSTR(str);

	while (1)
	{
		sprintf(str,"blinking %"PRIu32"\r\n",cont);
		cont++;
		DEBUGSTR(str);

		Board_LED_Toggle(LED);
		pausems(DELAY_MS);
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
