#include<iostream>
#include "huffman.h"
#include<vector>
#include<fstream>

int main( int argc, char* argv[]) {
    std::string inputFileName;
    if ( argc < 2 ) {
        std::cout << "Input file name: ";
        std::cin >> inputFileName;
    }
    else
        inputFileName = std::string( argv[1] );

    std::cout << "Decompression in progress..." << std::endl;

    std::vector<std::string> readText;
    HuffmanCompression::decompressFile( inputFileName, readText );

    std::cout << "Decompressed successfully" << std::endl;

    std::string outputFileName = inputFileName;
    int dotPos = inputFileName.find('.');
    int separatorPos = inputFileName.find_last_of( '/' );
    if( dotPos != std::string::npos )
        outputFileName = outputFileName.substr(0, dotPos);
    if( separatorPos != std::string::npos )
        outputFileName = outputFileName.substr( separatorPos );
    outputFileName = "output/" + outputFileName + "-decompressed.txt";

    std::ofstream outputFile( outputFileName );
    if ( outputFile.is_open() ) {
        for( std::string line : readText )
            outputFile << line;
        outputFile.close();
        std::cout << "Saved to '" << outputFileName << "'" << std::endl;
    }
    else {
        std::cerr << "Unable to open output file '" << outputFileName << "'" << std::endl;
        return 1;
    }

    return 0;
}
