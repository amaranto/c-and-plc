#include "stdint.h"
#include "inttypes.h"
#include "board.h"
#include <stdlib.h>
#include "../inc/functions.h"
#include "../inc/uart.h"


uint32_t check_input ( uint8_t* buffer ){


	uint32_t i =0,
			 read=0,
			 result =0,
			 len = sizeof (buffer) / sizeof ( uint8_t );
	uint8_t c;

	result = (uint32_t) strtol(buffer, NULL, 16);

	for ( i=1; i < len-1 ; i++ ) {

		c = *(buffer + read );
		if ( ( c >= 48 && c <= 58 ) ||  ( c >= 65 && c <= 70) || ( c >= 97 && c <= 102) ){
			read += sizeof ( uint8_t );
		}
		else {
			result = 256;
			break;
		}
	}

	return result;
}
