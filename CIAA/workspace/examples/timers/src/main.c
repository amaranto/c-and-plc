/* Copyright 2016, Pablo Ridolfi
 * Copyright 2016, Gustavo Muro
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
 
/** @brief This is a simple timer example.
 */

/** \addtogroup Timer Bare-metal Timer example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "main.h"
#include "board.h"
#include "stdint.h"
#include "inttypes.h"
#include "timer_18xx_43xx.h"

/*==================[macros and definitions]=================================*/

#define TIMER_MATCH_LED_1		LPC_TIMER0
#define MATCH_CHANNEL_LED_1		2

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();

	Chip_SCU_PinMux(2,10,0,FUNC1);

	Chip_TIMER_Init(TIMER_MATCH_LED_1);

	Chip_TIMER_PrescaleSet(TIMER_MATCH_LED_1, SystemCoreClock/1000);

	Chip_TIMER_SetMatch(TIMER_MATCH_LED_1, MATCH_CHANNEL_LED_1, 1000);

	Chip_TIMER_MatchEnableInt(TIMER_MATCH_LED_1, MATCH_CHANNEL_LED_1);

	Chip_TIMER_ResetOnMatchEnable(TIMER_MATCH_LED_1, MATCH_CHANNEL_LED_1);

	Chip_TIMER_ExtMatchControlSet(TIMER_MATCH_LED_1, 0, TIMER_EXTMATCH_TOGGLE, MATCH_CHANNEL_LED_1);

	Chip_TIMER_Enable(TIMER_MATCH_LED_1);

	NVIC_EnableIRQ(TIMER0_IRQn);
}



/*==================[external functions definition]==========================*/

void TIMER0_IRQHandler(void)
{
	Chip_TIMER_ClearMatch(TIMER_MATCH_LED_1, MATCH_CHANNEL_LED_1);

	Board_LED_Toggle(LED);
}

int main(void)
{
	initHardware();

	while (1)
	{

	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
