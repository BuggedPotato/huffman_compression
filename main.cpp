#include<iostream>
#include "charFrequency.cpp"
#include "huffman.h"
#include<vector>
#include<fstream>

int main( int argc, char* argv[]) {
    std::cout << "Initializing..." << std::endl;
    std::vector<std::string> letters;
    std::vector<double> values;
    for( int i = 65; i < 70; i++ ){
        if( CHAR_FREQUENCY[i] == 0 )
            continue;
        letters.push_back( std::string(1, char(i)) );
        values.push_back( CHAR_FREQUENCY[i] );
    }

    std::vector<TreeNode*> initArr = HuffmanCompression::getInitArray( letters, values, 2 );
    for( TreeNode* node : initArr )
        std::cout << node->sequence << ": " << node->probability << std::endl;
    std::cout << letters.size() << std::endl;
    std::cout << initArr.size() << std::endl;

    TreeNode *tree = HuffmanCompression::buildHuffmanTree( initArr );
    std::cout << "Compression tree built successfully." << std::endl;

    std::string inputFileName;
    if ( argc < 2 ) {
        std::cout << "Input file name: ";
        std::cin >> inputFileName;
    }
    else
        inputFileName = std::string( argv[1] );

    std::ifstream inputFile( inputFileName );
    if ( inputFile.is_open() ) {


        inputFile.close();
    }
    else {
        std::cerr << "Unable to open input file '" << inputFileName << "'" << std::endl;
        HuffmanCompression::destroyTree( tree );
        return 1;
    }

    HuffmanCompression::destroyTree( tree );

    return 0;
}