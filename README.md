# Huffman coding
Simple implementation of Huffman coding algorithm in the form of ASCII text
compression and decompression applications.

## Building
Run make. That's it. Creates two binaries "compress" and "decompress".

## Compressed file structure
Described in "fileStructure.md"

## Supported characters
File "ascii_freq_ARRAY.txt" contains an array form list of probabilities used
for building a tree. Index of each value is the ASCII code of a character.
All characters with a not zero probabilty are supported.

## Usage
compress FILENAME [WORD_LENGTH=1]
decompress FILENAME

## Notes
The program is not able to finish
within a reasonable time limit with a wordLength > 2.