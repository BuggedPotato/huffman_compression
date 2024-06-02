#include "huffman.h"
#include "TreeNode.h"
#include "BitReading.h"
#include<iostream>
#include<fstream>
#include<cstring>
#include<algorithm>
#include<stdexcept>
#include<cmath>

typedef union BInt32 {
    int32_t number;
    unsigned char bytes[4];
} BInt32;

TreeNode *nodeFromBytes( const unsigned char &elementSize, unsigned char *start, unsigned char *end );
void createAlphabet( std::vector<TreeNode *> &alphabet, TreeNode prevWord, std::vector<TreeNode *> &letters, int wordLength = 1 );
bool treeNodeCompare( const TreeNode *a, const TreeNode *b );

TreeNode *readTree( std::vector<unsigned char> &data, int &newBegin );
TreeNode *treeFromBytes( const unsigned char &elementSize, unsigned char** start, unsigned char **end );
bool readText( TreeNode* tree, int elementCount, unsigned char* start, std::vector<std::string> &text );

void addTreeFileData( TreeNode* head, std::vector<std::string> &text, std::vector<unsigned char> &buffer );
void addTextData(std::vector<std::string> &text, const int &sequenceSize, TreeNode* tree, std::vector<unsigned char> &data);

unsigned char getSequenceLength(TreeNode* head);
void getSequenceCodes( TreeNode* tree, std::vector<std::string> &sequences, std::vector<std::vector<bool>> &codes );
void getTreeByteSizeRec( TreeNode* head);
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


void addTreeFileData( TreeNode* head, std::vector<std::string> &text, std::vector<unsigned char> &buffer ) {
    if( !head ){
        std::cerr << "Tree is empty!" << std::endl;
        return;
    }
    const unsigned char elementSize = getSequenceLength(head);

    // v buffer for tree data <= tree size has to go in first
    std::vector<unsigned char> rawTreeData;
    BInt32 treeSize;
    treeSize.number = getTreeByteSize( head, rawTreeData );

    for(int i = 0; i < 4; i++)
        buffer.push_back( treeSize.bytes[i] );
    buffer.push_back(elementSize);

    for( int i = 0; i < rawTreeData.size(); i++ )
        buffer.push_back( rawTreeData.at(i) );

    // calculate raw elements count
    BInt32 elementCount;
    elementCount.number = 0;
    for( std::string line : text )
        elementCount.number += std::ceil( float(line.length()) / elementSize );
    for( int i = 0; i < 4; i++ )
        buffer.push_back(elementCount.bytes[i]);
}

bool HuffmanCompression::compressToFile( TreeNode* tree, std::vector<std::string> &text, std::string fileName ) {
    std::ofstream file( fileName, std::ios::binary );
    if ( !file.is_open() ) {
        std::cerr << "Could not open file '" << fileName << "'" << std::endl;
        return false;
    }

    std::vector<unsigned char> data;
    unsigned char sequenceLength = getSequenceLength(tree);
    addTreeFileData(tree, text, data);
    addTextData( text, sequenceLength, tree, data );

    file.write((char *)data.data(), data.size());
    file.close();
    return true;
}

bool HuffmanCompression::decompressFile( std::string fileName, std::vector<std::string> &text ){
    std::ifstream file( fileName, std::ios::binary );
    if ( !file.is_open() ) {
        return false;
    }
    std::vector<unsigned char> data( std::istreambuf_iterator<char>( file ), {} );
    file.close();

    int newBegin = 0;
    TreeNode* tree = readTree( data, newBegin );

    int32_t elementCount = 0;
    std::memcpy( &elementCount, data.data() + newBegin, sizeof(elementCount) );
    newBegin += sizeof(elementCount);

    bool res = readText( tree, elementCount, data.data() + newBegin, text );
    destroyTree(tree);
    return res;
}

bool readText( TreeNode* tree, int elementCount, unsigned char* start, std::vector<std::string> &text ){
    if( !tree ){
        std::cerr << "Tree is empty!" << std::endl;
        return false;
    }

    unsigned char currentByte = *start;
    int positionInByte = 8;
    for( elementCount; elementCount > 0; elementCount-- ){
        TreeNode* head = tree;
        while(true){
            if( !head->sequence.empty() ){
                text.push_back(head->sequence);
                break;
            }

            positionInByte--;
            if ( positionInByte < 0 ) {
                start++;
                positionInByte = 7;
                currentByte = *start ;
            }

            if( !BitReading::getBit( positionInByte, currentByte ) ){
                head = head->left;
            }
            else{
                head = head->right;
            }
        }
    }
    return true;
}


TreeNode *readTree( std::vector<unsigned char> &data, int &newBegin ) {
    int32_t treeSize;
    std::memcpy( &treeSize, data.data(), sizeof( treeSize ) );
    const unsigned char elementSize = data.at( sizeof( treeSize ) );
    newBegin = treeSize;

    unsigned char* start = ( data.data() + sizeof( treeSize ) + 1 );
    unsigned char* end = (data.data() + treeSize);

    TreeNode *head = treeFromBytes( elementSize, &start, &end );
    return head;
}


TreeNode *treeFromBytes( const unsigned char &elementSize, unsigned char** start, unsigned char **end ) {
    static unsigned char currentByte = **start;
    static int positionInByte = 8;

    positionInByte--;
    if ( positionInByte < 0 ) {
        (*start)++;
        if ( *start >= *end )
            return nullptr;
        positionInByte = 7;
        currentByte = **start ;
    }

    TreeNode *node = new TreeNode {};
    if ( BitReading::getBit(positionInByte, currentByte) ) {
        node->left = treeFromBytes( elementSize, start, end );
        node->right = treeFromBytes( elementSize, start, end );
    }
    else {
        unsigned char elementParts[elementSize] = {0};
        // read sequence characters bit by bit
        for( int i = 0; i < elementSize; i++ ){
            for( int j = 7; j >= 0; j-- ){
                positionInByte--;
                if ( positionInByte < 0 ) {
                    (*start)++;
                    if ( *start >= *end )
                        return nullptr;
                    positionInByte = 7;
                    currentByte = **start;
                }

                if( BitReading::getBit( positionInByte, currentByte ) )
                    elementParts[i] |= BitReading::POSITIONS[j];
            }
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
            if( match == sequences.size() ){
                std::string failedChars;
                for( char c : currentSequence )
                    failedChars += std::to_string( int(c) ) + ", ";
                throw std::invalid_argument("Sequence '" + currentSequence + "' (codes: [" + failedChars + "]) not found in tree");
            }

            std::vector<bool> code = codes.at(match);
            for( int i = 0; i < code.size(); i++ ){
                positionInByte--;
                if( positionInByte < 0 ){
                    data.push_back(currentByte);
                    positionInByte = 7;
                    currentByte = 0;
                }
                if( code.at(i) ){
                    currentByte |= BitReading::POSITIONS[positionInByte];
                }
            }
        }
    }
    data.push_back(currentByte);
}

void getSequenceCodes( TreeNode* tree, std::vector<std::string> &sequences, std::vector<std::vector<bool>> &codes ){
    static std::vector<bool> code;
    if( !tree->sequence.empty() ){
        sequences.push_back(tree->sequence);
        codes.push_back(code);
        code.pop_back();
    }
    else{
        if( tree->left ){
            code.push_back(0);
            getSequenceCodes(tree->left, sequences, codes);
        }
        if( tree->right ){
            code.push_back(1);
            getSequenceCodes(tree->right, sequences, codes);
            code.pop_back();
        }
    }
}


// returns leftover byte after finishing
unsigned char getTreeByteSizeRec( std::vector<unsigned char> &data, TreeNode* head){
    static unsigned char currentByte = 0;
    static int positionInByte = 8;

    positionInByte--;
    if ( positionInByte < 0 ) {
        data.push_back(currentByte);
        currentByte = 0;
        positionInByte = 7;
    }
    if( head->sequence.empty() ){
        if(head->left){
            currentByte |= BitReading::POSITIONS[positionInByte];
            getTreeByteSizeRec( data, head->left);
        }
        if(head->right){
            getTreeByteSizeRec( data, head->right);
        }
    }
    else{
        int elementSize = head->sequence.length();
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
    }
    return currentByte;
}

int32_t getTreeByteSize(TreeNode* head, std::vector<unsigned char> &data){
    int32_t size = 0;
    unsigned char leftover = getTreeByteSizeRec( data, head);
    data.push_back(leftover);
    size += data.size();
    size += 5; // 4 for tree size 1 for element size
    return size;
}


void HuffmanCompression::printTree( TreeNode* head ){
    if( !head->sequence.empty() ){
        std::cout << head->sequence << std::endl;
        return;
    }
    std::cout << "NODE" << std::endl;
    printTree(head->left);
    printTree(head->right);
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
