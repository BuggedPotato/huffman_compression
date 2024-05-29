#pragma once
#include<string>

typedef struct TreeNode{
    double probability = 1;
    std::string sequence;
    TreeNode *left = nullptr; // 0
    TreeNode *right = nullptr; // 1
} TreeNode;