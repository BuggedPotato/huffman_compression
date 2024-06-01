#include "BitReading.h"

bool* BitReading::getBits(unsigned char value){
	bool *res = new bool[8];
	for( int i = 0; i < 8; i++ )
		res[i] = getBit( i, value );
	return res;
}

bool BitReading::getBit( unsigned char position, unsigned char value ) {
	return ( value & POSITIONS[position] ) != 0;
}
