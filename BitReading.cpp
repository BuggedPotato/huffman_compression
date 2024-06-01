#include "BitReading.h"

const BitReading::BitPosition POSITIONS[8] = {
		BitReading::ZERO,
		BitReading::ONE,
		BitReading::TWO,
		BitReading::THREE,
		BitReading::FOUR,
		BitReading::FIVE,
		BitReading::SIX,
		BitReading::SEVEN
};


bool* BitReading::getBits(unsigned char value){
	bool *res = new bool[8];
	for( int i = 0; i < 8; i++ )
		res[i] = getBit( i, value );
	return res;
}

bool BitReading::getBit( unsigned char position, unsigned char value ) {
	return ( value & POSITIONS[position] ) != 0;
}
