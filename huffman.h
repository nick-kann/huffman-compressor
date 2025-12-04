#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <memory>

struct Node {
    char ch;
    int freq;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    
    Node(char c, int f);
    Node(int f, std::unique_ptr<Node> l, std::unique_ptr<Node> r);
};

void buildCodes(Node* root, std::string code, std::unordered_map<char, std::string>& codes);
void serializeTree(Node* root, std::ofstream& out);
std::unique_ptr<Node> deserializeTree(std::ifstream& in);

#endif

