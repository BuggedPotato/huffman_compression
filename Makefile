all: compression
compression: main.o huffman.o BitReading.o
	g++ -o compression main.o huffman.o BitReading.o
main.o: main.cpp huffman.h TreeNode.h
	g++ -c main.cpp
huffman.o: huffman.cpp TreeNode.h BitReading.h
	g++ -c huffman.cpp
BitReading.o:
	g++ -c BitReading.cpp
clean:
	rm *.o
