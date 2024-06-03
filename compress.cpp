#include<iostream>
#include "charFrequency.cpp"
#include "huffman.h"
#include<vector>
#include<fstream>

int main( int argc, char* argv[]) {
    std::cout << "Initializing..." << std::endl;
    std::vector<std::string> letters;
    std::vector<double> values;
    for( int i = 0; i < FREQUENCY_SIZE; i++ ){
        if( CHAR_FREQUENCY[i] == 0 )
            continue;
        letters.push_back( std::string(1, char(i)) );
        values.push_back( CHAR_FREQUENCY[i] );
    }

    int sequenceLength = 1;
    std::string inputFileName;
    if ( argc < 2 ) {
        std::cout << "Input file name: ";
        std::cin >> inputFileName;
    }
    else
        inputFileName = std::string( argv[1] );

    if( argc >= 3 )
        sequenceLength = std::stoi(argv[2]);

    std::vector<TreeNode*> initArr = HuffmanCompression::getInitArray( letters, values, sequenceLength );

    TreeNode *tree = HuffmanCompression::buildHuffmanTree( initArr );
    std::cout << "Compression tree built successfully." << std::endl;

    std::vector<std::string> text;
    std::ifstream inputFile( inputFileName );
    if ( inputFile.is_open() ) {
        std::string line;
        while(std::getline(inputFile, line)){
            text.push_back(line + char(10)); // adds newline
        }
        inputFile.close();

        std::string outputFileName = inputFileName;
        int dotPos = inputFileName.find_last_of('.');
        int separatorPos = inputFileName.find_last_of( '/' );
        if( dotPos != std::string::npos )
            outputFileName = outputFileName.substr(0, dotPos);
        if( separatorPos != std::string::npos )
            outputFileName = outputFileName.substr( separatorPos );
        outputFileName += "-compressed.bin";
        outputFileName = "output/" + outputFileName;

        if( HuffmanCompression::compressToFile( tree, text, outputFileName ) )
            std::cout << "Successfully compressed into '" + outputFileName + "'" << std::endl;
        else
            std::cerr << "Compression failed" << std::endl;
    }
    else {
        std::cerr << "Unable to open input file '" << inputFileName << "'" << std::endl;
        HuffmanCompression::destroyTree( tree );
        return 1;
    }
    HuffmanCompression::destroyTree( tree );

    return 0;
}
