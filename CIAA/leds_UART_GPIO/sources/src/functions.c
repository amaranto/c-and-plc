#include "stdint.h"
#include "inttypes.h"
#include "board.h"
#include <stdlib.h>
#include "../inc/functions.h"
#include "../inc/uart.h"


uint32_t interpolate ( uint32_t input_value ){

	uint32_t result = 0;

	if ( input_value >= 0x40 && input_value < 0x80 )
		result = input_value *2;
	else if ( input_value == 0x80 )
		result = 0xff;
	else if ( input_value > 0x80 && input_value <= 0xc0 )
		result = input_value / 3;
	else
		result = input_value;

	return result;
}

uint32_t check_input ( uint8_t* buffer ){


	uint32_t i =0,
			 read=0,
			 result =0,
			 len = sizeof (buffer) / sizeof ( uint8_t );
	uint8_t c;

	for ( i=1; i < len-1 ; i++ ) {

		c = *(buffer + read );

		if ( ( c >= 48 && c <= 58 ) ||  ( c >= 65 && c <= 70) || ( c >= 97 && c <= 102) ){
			read += sizeof ( uint8_t );
		}
		else
			return 256;

	}

	return (uint32_t) strtol(buffer, NULL, 16);
}



