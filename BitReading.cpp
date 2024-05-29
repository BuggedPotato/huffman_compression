#include "BitReading.h"

inline bool BitReading::getBit( unsigned char position, unsigned char value ) {
	return ( value & POSITIONS[position] ) != 0;
}

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