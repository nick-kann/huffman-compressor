#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <unordered_map>
#include <fstream>

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    
    Node(char c, int f);
    Node(int f, Node* l, Node* r);
};

void buildCodes(Node* root, std::string code, std::unordered_map<char, std::string>& codes);
void serializeTree(Node* root, std::ofstream& out);
Node* deserializeTree(std::ifstream& in);
void deleteTree(Node* root);

#endif

