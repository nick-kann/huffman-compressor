#include "compressor.h"
#include "huffman.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <bitset>
#include <memory>

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

class BitWriter {
    std::ofstream& out;
    unsigned char buffer;
    int bitCount;
    
public:
    BitWriter(std::ofstream& o) : out(o), buffer(0), bitCount(0) {}
    
    void writeBit(bool bit) {
        buffer |= (bit << (7 - bitCount));
        bitCount++;
        
        if (bitCount == 8) {
            out.put(buffer);
            buffer = 0;
            bitCount = 0;
        }
    }
    
    void writeBits(const std::string& bits) {
        for (char bit : bits) {
            writeBit(bit == '1');
        }
    }
    
    void flush() {
        if (bitCount > 0) {
            out.put(buffer);
            buffer = 0;
            bitCount = 0;
        }
    }
    
    int getPadding() {
        return bitCount > 0 ? (8 - bitCount) : 0;
    }
};

class BitReader {
    std::ifstream& in;
    unsigned char buffer;
    int bitCount;
    int padding;
    bool hasMore;
    std::streampos fileEnd;
    
public:
    BitReader(std::ifstream& i, int pad) : in(i), buffer(0), bitCount(8), padding(pad), hasMore(true) {
        std::streampos current = in.tellg();
        in.seekg(0, std::ios::end);
        fileEnd = in.tellg();
        in.seekg(current);
    }
    
    bool readBit() {
        if (bitCount == 8) {
            if (!in.get(reinterpret_cast<char&>(buffer))) {
                hasMore = false;
                return false;
            }
            bitCount = 0;
        }
        
        bool bit = (buffer >> (7 - bitCount)) & 1;
        bitCount++;
        return bit;
    }
    
    bool isEof() {
        if (!hasMore) return true;
        
        std::streampos current = in.tellg();
        if (current >= fileEnd) {
            return bitCount >= (8 - padding);
        }
        return false;
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
    
    std::vector<std::unique_ptr<Node>> nodes;
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    
    for (auto& pair : freq) {
        nodes.push_back(std::make_unique<Node>(pair.first, pair.second));
        pq.push(nodes.back().get());
    }
    
    while (pq.size() > 1) {
        Node* leftPtr = pq.top();
        int leftFreq = leftPtr->freq;
        pq.pop();
        Node* rightPtr = pq.top();
        int rightFreq = rightPtr->freq;
        pq.pop();
        
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (it->get() == leftPtr) {
                left = std::move(*it);
                nodes.erase(it);
                break;
            }
        }
        
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (it->get() == rightPtr) {
                right = std::move(*it);
                nodes.erase(it);
                break;
            }
        }
        
        nodes.push_back(std::make_unique<Node>(leftFreq + rightFreq, 
            std::move(left), std::move(right)));
        pq.push(nodes.back().get());
    }
    
    std::unique_ptr<Node> root = std::move(nodes.back());
    std::unordered_map<char, std::string> codes;
    buildCodes(root.get(), "", codes);
    
    in.open(inputFile, std::ios::binary);
    std::ofstream out(outputFile, std::ios::binary);
    
    serializeTree(root.get(), out);
    
    std::streampos paddingPos = out.tellp();
    out.put(0);
    
    BitWriter bitWriter(out);
    
    while (in.get(ch)) {
        bitWriter.writeBits(codes[ch]);
    }
    
    int padding = bitWriter.getPadding();
    bitWriter.flush();
    
    out.seekp(paddingPos);
    out.put(padding);
    
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
    
    auto root = deserializeTree(in);
    
    int padding = in.get();
    
    BitReader bitReader(in, padding);
    std::ofstream out(outputFile, std::ios::binary);
    Node* current = root.get();
    
    while (!bitReader.isEof()) {
        bool bit = bitReader.readBit();
        
        if (bit) {
            current = current->right.get();
        } else {
            current = current->left.get();
        }
        
        if (!current->left && !current->right) {
            out.put(current->ch);
            current = root.get();
        }
    }
    
    in.close();
    out.close();
    
    std::cout << "decompressed: " << inputFile << " -> " << outputFile << std::endl;
}

