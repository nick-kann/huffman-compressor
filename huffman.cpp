#include "huffman.h"
#include <iostream>

Node::Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
Node::Node(int f, Node* l, Node* r) : ch(0), freq(f), left(l), right(r) {}

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
    
    buildCodes(root->left, code + "0", codes);
    buildCodes(root->right, code + "1", codes);
}

void serializeTree(Node* root, std::ofstream& out) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        out.put('1');
        out.put(root->ch);
    } else {
        out.put('0');
        serializeTree(root->left, out);
        serializeTree(root->right, out);
    }
}

Node* deserializeTree(std::ifstream& in) {
    char bit = in.get();
    if (bit == '1') {
        char ch = in.get();
        return new Node(ch, 0);
    } else {
        Node* left = deserializeTree(in);
        Node* right = deserializeTree(in);
        return new Node(0, left, right);
    }
}

void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

