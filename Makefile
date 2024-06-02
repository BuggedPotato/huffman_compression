all: compression decompression
decompression: decompress.o huffman.o BitReading.o
	g++ -o decompress decompress.o huffman.o BitReading.o
compression: compress.o huffman.o BitReading.o
	g++ -o compress compress.o huffman.o BitReading.o
main.o: compress.cpp huffman.h TreeNode.h
	g++ -c compress.cpp
huffman.o: huffman.cpp TreeNode.h BitReading.h
	g++ -c huffman.cpp
BitReading.o:
	g++ -c BitReading.cpp
clean:
	rm *.o
