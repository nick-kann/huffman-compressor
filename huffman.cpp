#include "huffman.h"
#include <iostream>

Node::Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
Node::Node(int f, std::unique_ptr<Node> l, std::unique_ptr<Node> r) : ch(0), freq(f), left(std::move(l)), right(std::move(r)) {}

void buildCodes(Node* root, std::string code, std::unordered_map<char, std::string>& codes) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        if (!code.empty()) {
            codes[root->ch] = code;
        } else {
            codes[root->ch] = "0";
        }
        return;
    }
    
    buildCodes(root->left.get(), code + "0", codes);
    buildCodes(root->right.get(), code + "1", codes);
}

void serializeTree(Node* root, std::ofstream& out) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        out.put('1');
        out.put(root->ch);
    } else {
        out.put('0');
        serializeTree(root->left.get(), out);
        serializeTree(root->right.get(), out);
    }
}

std::unique_ptr<Node> deserializeTree(std::ifstream& in) {
    char bit = in.get();
    if (bit == '1') {
        char ch = in.get();
        return std::make_unique<Node>(ch, 0);
    } else {
        auto left = deserializeTree(in);
        auto right = deserializeTree(in);
        return std::make_unique<Node>(0, std::move(left), std::move(right));
    }
}

