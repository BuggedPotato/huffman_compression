#include "huffman.h"
#include "TreeNode.h"
#include<bitset>
#include<iostream>
#include<fstream>
#include<algorithm>

#define BYTE_SIZE 8

TreeNode *nodeFromBytes( unsigned char &elementSize, unsigned char *start, unsigned char *end );
void createAlphabet( std::vector<TreeNode *> &alphabet, TreeNode prevWord, std::vector<TreeNode *> &letters, int wordLength = 1 );
bool treeNodeCompare( const TreeNode *a, const TreeNode *b );

std::vector<TreeNode*> HuffmanCompression::getInitArray( std::vector<std::string> &letters, std::vector<double> &values, int wordLength ){
    std::vector<TreeNode*> treeLetters;
    for ( int i = 0; i < letters.size(); i++ ) {
        TreeNode *tmp = new TreeNode {};
        tmp->probability = values.at( i );
        tmp->sequence = letters.at( i );
        treeLetters.emplace_back( tmp );
    }
    if ( wordLength <= 1 ) {
        return treeLetters;
    }
    std::vector<TreeNode*> alphabet;
    createAlphabet( alphabet, { 1, "" }, treeLetters, wordLength );
    return alphabet;
}

TreeNode *HuffmanCompression::buildHuffmanTree( std::vector<TreeNode*> &initArr ) {
    while ( initArr.size() > 1 ) {
        std::sort( initArr.begin(), initArr.end(), treeNodeCompare );
        TreeNode* n1 = initArr.back();
        initArr.pop_back();
        TreeNode* n2 = initArr.back();
        initArr.pop_back();
        TreeNode *newNode = new TreeNode {};
        newNode->probability = n1->probability + n2->probability;
        newNode->left = n1;
        newNode->right = n2;
        initArr.emplace_back( newNode );
    }
    return initArr.at( 0 );
}

TreeNode *HuffmanCompression::readTreeFromFile( std::string fileName ) {
    std::ifstream file( fileName, std::ios::binary );
    if ( !file.is_open() ) {
        return nullptr;
    }
    std::vector<unsigned char> buffer( std::istreambuf_iterator<char>( file ), {} );
    __int32 treeSize;
    std::memcpy( &treeSize, buffer.data(), sizeof( __int32 ) );
    unsigned char elementSize = buffer.at( 4 );
    // TODO raw data element bit size
    // int readByte = 5;
    // first from left is 0, last is 7
    // skip first bit assuming first node exists
    // unsigned char positionInByte = 1;
    // unsigned char mask = 1 << positionInByte;

    TreeNode *head = nodeFromBytes( elementSize, &( *( buffer.begin() + 4 ) ), &( *buffer.end() ) );
    

    file.close();
    return nullptr;
}

TreeNode *nodeFromBytes( unsigned char &elementSize, unsigned char* start, unsigned char *end ) {
    static std::bitset<BYTE_SIZE> currentByte { *start };
    static unsigned char positionInByte = 0;

    if ( positionInByte > 7 ) {
        start++;
        if ( start >= end )
            return nullptr;
        positionInByte = 0;
        currentByte = { *start };
    }

    TreeNode *node = new TreeNode {};
    if ( currentByte[positionInByte] ) {
        node->left = nodeFromBytes( ++positionInByte, start, end );
        node->right = nodeFromBytes( ++positionInByte, start, end );
    }
    else {
        positionInByte++;
        const int elementBitSize = elementSize * BYTE_SIZE;
        std::bitset<elementBitSize> element ( start, positionInByte, elementBitSize );
        start += elementSize;
        node->sequence = element.to_string();
    }
    return node;
}


void HuffmanCompression::destroyTree( TreeNode *&head ) {
    // always has two children
    if ( head->left )
        HuffmanCompression::destroyTree( head->left );
    if ( head->right )
        HuffmanCompression::destroyTree( head->right );
    delete head;
    head = nullptr;
}

void createAlphabet( std::vector<TreeNode *> &alphabet, TreeNode prevWord, std::vector<TreeNode *> &letters, int wordLength ) {
    if ( prevWord.sequence.length() == wordLength ) {
        alphabet.emplace_back( new TreeNode { prevWord } );
        return;
    }
    for ( int i = 0; i < letters.size(); i++ ) {
        TreeNode tmp;
        tmp.sequence = prevWord.sequence + letters.at( i )->sequence;
        tmp.probability = prevWord.probability * letters.at( i )->probability;
        createAlphabet( alphabet, tmp, letters, wordLength );
    }
}
// DESCENDING
bool treeNodeCompare( const TreeNode *a, const TreeNode *b ) {
    return a->probability > b->probability;
}