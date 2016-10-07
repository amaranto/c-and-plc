#include "board.h"
#include "stdint.h"
#include "inttypes.h"

#ifndef BUTTONS_H_
	#define BUTTONS_H_

	/*==================[macros]=================================================*/
	#define BUTTON_1 0
	#define BUTTON_2 1
	#define BUTTON_3 2
	#define BUTTON_4 3

	#define TOTAL_BUTTONS 4
	#define NO_BUTTON_PRESSED   0x00


	void Buttons_InitAll(void);
	uint32_t Buttons_GetStat( int32_t button );

#endif
