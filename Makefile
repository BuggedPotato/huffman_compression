all: compression
compression: main.o huffman.o
	g++ -o compression main.o huffman.o
main.o: main.cpp huffman.h TreeNode.h
	g++ -c main.cpp
huffman.o: huffman.cpp TreeNode.h
	g++ -c huffman.cpp
clean:
	rm *.o
