#pragma once
namespace BitReading {
	enum BitPosition {
		ZERO = 0b00000001,
		ONE = 0b00000010,
		TWO = 0b00000100,
		THREE = 0b00001000,
		FOUR = 0b00010000,
		FIVE = 0b00100000,
		SIX = 0b01000000,
		SEVEN = 0b10000000,
	};

	const BitPosition POSITIONS[8] = {
		BitReading::ZERO,
		BitReading::ONE,
		BitReading::TWO,
		BitReading::THREE,
		BitReading::FOUR,
		BitReading::FIVE,
		BitReading::SIX,
		BitReading::SEVEN
	};
	bool* getBits(unsigned char value);
	bool getBit( unsigned char position, unsigned char value );
}
