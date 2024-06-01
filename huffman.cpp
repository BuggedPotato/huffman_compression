#include "huffman.h"
#include "TreeNode.h"
#include "BitReading.h"
#include<iostream>
#include<fstream>
#include<cstring>
#include<algorithm>
#include<stdexcept>
#include<cmath>

#define BYTE_SIZE 8

TreeNode *nodeFromBytes( const unsigned char &elementSize, unsigned char *start, unsigned char *end );
void createAlphabet( std::vector<TreeNode *> &alphabet, TreeNode prevWord, std::vector<TreeNode *> &letters, int wordLength = 1 );
bool treeNodeCompare( const TreeNode *a, const TreeNode *b );

void addTreeFileData( TreeNode* head, std::vector<unsigned char> &buffer );
void addTextData(std::vector<std::string> &text, const int &sequenceSize, TreeNode* tree, std::vector<unsigned char> &data);
unsigned char getSequenceLength(TreeNode* head);
void getSequenceCodes( TreeNode* tree, std::vector<std::string> &sequences, std::vector<std::vector<bool>> &codes );
void getTreeByteSizeRec(int32_t &size, int &bitCounter, TreeNode* head);
int32_t getTreeByteSize(TreeNode* head, std::vector<unsigned char> &data);

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


void addTreeFileData( TreeNode* head, std::vector<unsigned char> &buffer ) {
    if( !head ){
        std::cerr << "Tree is empty!" << std::endl;
        return;
    }
    const unsigned char elementSize = getSequenceLength(head);
    union BInt32 {
        int32_t number;
        unsigned char bytes[4];
    } treeSize;
    std::vector<unsigned char> rawTreeData;
    treeSize.number = getTreeByteSize( head, rawTreeData );

    for(int i = 0; i < 4; i++)
        buffer.push_back( treeSize.bytes[i] );
    buffer.push_back(elementSize);

    for( int i = 0; i < rawTreeData.size(); i++ )
        buffer.push_back( rawTreeData.at(i) );
}

bool HuffmanCompression::compressToFile( TreeNode* tree, std::vector<std::string> text, std::string fileName ) {
    std::ofstream file( fileName, std::ios::binary );
    if ( !file.is_open() ) {
        std::cerr << "Could not open file '" << fileName << "'" << std::endl;
        return false;
    }

    std::vector<unsigned char> data;
    addTreeFileData(tree, data);
    addTextData( text, getSequenceLength(tree), tree, data );

    file.write((char *)data.data(), data.size() /** sizeof(unsigned char)*/);
    file.close();
    return true;
}


TreeNode *HuffmanCompression::readTreeFromFile( std::string fileName ) {
    std::ifstream file( fileName, std::ios::binary );
    if ( !file.is_open() ) {
        return nullptr;
    }
    std::vector<unsigned char> buffer( std::istreambuf_iterator<char>( file ), {} );
    int32_t treeSize;
    std::memcpy( &treeSize, buffer.data(), sizeof( int32_t ) );
    const unsigned char elementSize = buffer.at( 4 );

    TreeNode *head = nodeFromBytes( elementSize, &( *( buffer.begin() + 5 ) ), &( *buffer.end() ) ); // TODO maybe +4?
    file.close();
    return head;
}

TreeNode *nodeFromBytes( const unsigned char &elementSize, unsigned char* start, unsigned char *end ) {
    static unsigned char currentByte = *start;
    static unsigned char positionInByte = 7;

    if ( positionInByte < 0 ) {
        start++;
        if ( start >= end )
            return nullptr;
        positionInByte = 7;
        currentByte = { *start };
    }

    TreeNode *node = new TreeNode {};
    if ( BitReading::getBit(positionInByte, currentByte) ) {
        positionInByte--;
        node->left = nodeFromBytes( elementSize, start, end );
        node->right = nodeFromBytes( elementSize, start, end );
    }
    else {
        std::string elementParts[elementSize];
        // read sequence characters bit by bit
        for( int i = 0; i < elementSize * 8; i++ ){
            positionInByte--;
            if ( positionInByte < 0 ) {
                start++;
                if ( start >= end )
                    return nullptr;
                positionInByte = 7;
                currentByte = { *start };
            }
            elementParts[i/8] += std::to_string(BitReading::getBit( positionInByte, currentByte ));
        }

        for( int i = 0; i < elementSize; i++ )
            node->sequence += elementParts[i];
    }
    return node;
}


unsigned char getSequenceLength(TreeNode* head){
    if( !head->sequence.empty() )
        return (unsigned char)head->sequence.length();
    if( head->left )
        return getSequenceLength(head->left);
    else if( head->right )
        return getSequenceLength(head->right);
    return -1;
}


void addTextData(std::vector<std::string> &text, const int &sequenceSize, TreeNode* tree, std::vector<unsigned char> &data){
    std::vector<std::string> sequences;
    std::vector<std::vector<bool>> codes;
    getSequenceCodes( tree, sequences, codes );

    unsigned char currentByte = 0;
    int positionInByte = 8;

    for( std::string line : text ){
        // extend string so it is divisible into sequences
        while( line.length() % sequenceSize ){
            line += " ";
        }

        for( int strStart = 0; strStart < line.length(); strStart += sequenceSize ){
            std::string currentSequence = line.substr(strStart, sequenceSize);

            int match = 0;
            for( match; match < sequences.size(); match++ )
                if( sequences.at(match) == currentSequence )
                    break;
            if( match == sequences.size() )
                throw std::invalid_argument("Sequence not found in tree");

            for( int i = 0; i < codes.at(match).size(); i++ ){
                positionInByte--;
                if( positionInByte < 0 ){
                    data.push_back(currentByte);
                    positionInByte = 7;
                    currentByte = 0;
                }
                if( codes.at(match).at(i) ){
                    currentByte |= BitReading::POSITIONS[positionInByte];
                }
            }
        }
    }
    if( currentByte )
        data.push_back(currentByte);
}

void getSequenceCodes( TreeNode* tree, std::vector<std::string> &sequences, std::vector<std::vector<bool>> &codes ){
    static std::vector<bool> code;
    if( !tree->sequence.empty() ){
        sequences.push_back(tree->sequence);
        codes.push_back(code);
        code.clear();
    }
    else{
        if( tree->left ){
            code.push_back(0);
            getSequenceCodes(tree->left, sequences, codes);
        }
        if( tree->right ){
            code.push_back(1);
            getSequenceCodes(tree->right, sequences, codes);
        }
    }
}


// returns leftover byte after finishing
unsigned char getTreeByteSizeRec(int32_t &size, /*int &bitCounter,*/ std::vector<unsigned char> &data, TreeNode* head){
    static unsigned char currentByte = 0;
    static int positionInByte = 8;

    // bitCounter++;
    positionInByte--;
    if ( positionInByte < 0 ) {
        data.push_back(currentByte);
        currentByte = 0;
        positionInByte = 7;
    }

    if( head->sequence.empty() ){
        if(head->left){
            currentByte |= BitReading::POSITIONS[positionInByte];
            getTreeByteSizeRec(size, /*bitCounter,*/ data, head->left);
            positionInByte--;
        }
        if(head->right){
            currentByte |= BitReading::POSITIONS[positionInByte];
            getTreeByteSizeRec(size, /*bitCounter,*/ data, head->right);
        }
    }
    else{
        int elementSize = head->sequence.length();
        // size += elementSize;
        for( int j = 0; j < elementSize; j++){
            for(int i = 7; i >= 0; i--){
                positionInByte--;
                if ( positionInByte < 0 ) {
                    data.push_back(currentByte);
                    currentByte = 0;
                    positionInByte = 7;
                }
                if( BitReading::getBit( i, (unsigned char)head->sequence[j] ) )
                    currentByte |= BitReading::POSITIONS[positionInByte];
            }
        }
        return currentByte;
    }
}

int32_t getTreeByteSize(TreeNode* head, std::vector<unsigned char> &data){
    // int bitCounter = 0;
    int32_t size = 0;
    unsigned char leftover = getTreeByteSizeRec(size, /*bitCounter,*/ data, head);
    if (leftover)
        data.push_back(leftover);
    // size += std::ceil(bitCounter / 8.);
    size += data.size();
    size += 5; // 4 for tree size and 1 for element size
    return size;
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
