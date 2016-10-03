/* Copyright 2015, Pablo Ridolfi
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

/** @brief PWM example with SCT.
 *
 * PWM signal on Board LED 0, period 0.1ms, duty between 0% and 100%.
 */

 /** \addtogroup pwm PWM example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "board.h"
#include "main.h"

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
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/1000);
    Board_Init();

    Chip_SCTPWM_Init(LPC_SCT);

    Chip_SCTPWM_SetRate(LPC_SCT, 10000);

    Chip_SCU_PinMux(2,0,0,FUNC1);
    Chip_SCU_PinMux(2,1,0,FUNC1);
    Chip_SCU_PinMux(2,2,0,FUNC1);

    Chip_SCU_PinMux(2,10,0,FUNC2);
    Chip_SCU_PinMux(2,11,0,FUNC2);
    Chip_SCU_PinMux(2,12,0,FUNC2);

    Chip_SCTPWM_SetOutPin(LPC_SCT, 1, 2);

    Chip_SCTPWM_Start(LPC_SCT);
}

static void pausems(uint32_t t)
{
   pausems_count = t;
   while(pausems_count != 0) {
      __WFI();
   }
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if (pausems_count != 0) {
      pausems_count--;
   }
}

int main(void)
{
   int duty = 0;
   int up = 1;

   initHardware();

   while(1)
   {
      pausems(20);

      if (up)
      {
    	  duty++;
    	  if (duty >= 100)
    		  up = 0;
      }
      else
      {
    	  duty--;
		  if (duty <= 0)
			  up = 1;
      }

      Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_PercentageToTicks(LPC_SCT, duty));
   }
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
