#pragma once
#include "TreeNode.h"
#include<vector>

namespace HuffmanCompression {
    std::vector<TreeNode*> getInitArray( std::vector<std::string> &letters, std::vector<double> &values, int wordLength );
    TreeNode* buildHuffmanTree( std::vector<TreeNode*> &initArr );
    
    bool compressToFile( TreeNode* tree, std::vector<std::string> &text, std::string fileName );
    bool decompressFile( std::string fileName, std::vector<std::string> &text );

    void printTree( TreeNode* head );
    void destroyTree( TreeNode* &head );
}
