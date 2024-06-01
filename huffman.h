#pragma once
#include "TreeNode.h"
#include<vector>

namespace HuffmanCompression {
    std::vector<TreeNode*> getInitArray( std::vector<std::string> &letters, std::vector<double> &values, int wordLength );
    TreeNode* buildHuffmanTree( std::vector<TreeNode*> &initArr );
    
    bool compressToFile( TreeNode* tree, std::vector<std::string> text, std::string fileName );
    TreeNode *readTreeFromFile( std::string fileName );
    // TreeNode *nodeFromBytes( unsigned const char &elementSize, unsigned char* start, unsigned char *end );

    // unsigned char getSequenceLength(TreeNode* head);
    // int32_t getTreeByteSize(TreeNode* head);

    void printTree( TreeNode* head );
    void destroyTree( TreeNode* &head );
}
