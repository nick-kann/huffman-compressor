#include "compressor.h"
#include "huffman.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <bitset>

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

void compress(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "cant open input file" << std::endl;
        return;
    }
    
    std::unordered_map<char, int> freq;
    char ch;
    while (in.get(ch)) {
        freq[ch]++;
    }
    in.close();
    
    if (freq.empty()) {
        std::cerr << "empty file" << std::endl;
        return;
    }
    
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    for (auto& pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }
    
    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();
        
        Node* merged = new Node(left->freq + right->freq, left, right);
        pq.push(merged);
    }
    
    Node* root = pq.top();
    std::unordered_map<char, std::string> codes;
    buildCodes(root, "", codes);
    
    in.open(inputFile, std::ios::binary);
    std::ofstream out(outputFile, std::ios::binary);
    
    serializeTree(root, out);
    
    std::string bitString;
    while (in.get(ch)) {
        bitString += codes[ch];
    }
    
    int padding = 8 - (bitString.length() % 8);
    if (padding != 8) {
        bitString += std::string(padding, '0');
        out.put(padding);
    } else {
        out.put(0);
    }
    
    for (size_t i = 0; i < bitString.length(); i += 8) {
        std::bitset<8> bits(bitString.substr(i, 8));
        out.put(bits.to_ulong());
    }
    
    deleteTree(root);
    in.close();
    out.close();
    
    std::cout << "compressed: " << inputFile << " -> " << outputFile << std::endl;
}

void decompress(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "cant open input file" << std::endl;
        return;
    }
    
    Node* root = deserializeTree(in);
    
    int padding = in.get();
    
    std::string bitString;
    char byte;
    while (in.get(byte)) {
        bitString += std::bitset<8>(byte).to_string();
    }
    
    if (padding > 0) {
        bitString = bitString.substr(0, bitString.length() - padding);
    }
    
    std::ofstream out(outputFile, std::ios::binary);
    Node* current = root;
    
    for (char bit : bitString) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        if (!current->left && !current->right) {
            out.put(current->ch);
            current = root;
        }
    }
    
    deleteTree(root);
    in.close();
    out.close();
    
    std::cout << "decompressed: " << inputFile << " -> " << outputFile << std::endl;
}

