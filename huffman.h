#pragma once
#include "TreeNode.h"
#include<vector>

namespace HuffmanCompression {
    std::vector<TreeNode*> getInitArray( std::vector<std::string> &letters, std::vector<double> &values, int wordLength );
    TreeNode* buildHuffmanTree( std::vector<TreeNode*> &initArr );
    
    TreeNode *readTreeFromFile( std::string fileName );
    TreeNode *nodeFromBytes( unsigned const char &elementSize, unsigned char* start, unsigned char *end );

    void destroyTree( TreeNode* &head );
}
