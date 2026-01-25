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
    std::vector<unsigned char> data;
    size_t currentBit;
    size_t totalBits;
    
public:
    BitReader(std::ifstream& in, int padding) {
        std::streampos startPos = in.tellg();
        in.seekg(0, std::ios::end);
        std::streampos endPos = in.tellg();
        in.seekg(startPos);
        
        size_t remainingBytes = endPos - startPos;
        
        if (remainingBytes == 0) {
            totalBits = 0;
            currentBit = 0;
            return;
        }
        
        data.resize(remainingBytes);
        in.read(reinterpret_cast<char*>(data.data()), remainingBytes);
        
        currentBit = 0;
        totalBits = data.size() * 8 - padding;
    }
    
    inline bool readBit() {
        size_t byteIdx = currentBit >> 3;
        unsigned char byte = data[byteIdx];
        int bitPos = 7 - (currentBit & 7);
        currentBit++;
        return (byte >> bitPos) & 1;
    }
    
    inline bool isEof() {
        return currentBit >= totalBits;
    }
};

void compress(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "cant open input file" << std::endl;
        return;
    }
    
    int freq[256] = {0};
    int byte;
    while ((byte = in.get()) != EOF) {
        freq[static_cast<unsigned char>(byte)]++;
    }
    in.close();
    
    bool hasData = false;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            hasData = true;
            break;
        }
    }
    
    if (!hasData) {
        std::cerr << "empty file" << std::endl;
        return;
    }
    
    std::vector<std::unique_ptr<Node>> nodes;
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            nodes.push_back(std::make_unique<Node>(static_cast<char>(i), freq[i]));
            pq.push(nodes.back().get());
        }
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
    
    while ((byte = in.get()) != EOF) {
        bitWriter.writeBits(codes[static_cast<char>(byte)]);
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
    
    const size_t bufferSize = 65536; // 64KB, fewer writes
    char buffer[bufferSize];
    size_t bufferPos = 0;

    if (!root->left && !root->right) {
        while (!bitReader.isEof()) {
            bitReader.readBit();
            buffer[bufferPos++] = root->ch;
            if (bufferPos >= bufferSize) {
                out.write(buffer, bufferSize);
                bufferPos = 0;
            }
        }

        if (bufferPos > 0) {
            out.write(buffer, bufferPos);
        }

        in.close();
        out.close();
        
        std::cout << "decompressed: " << inputFile << " -> " << outputFile << std::endl;
        return;
    }
    
    while (!bitReader.isEof()) {
        bool bit = bitReader.readBit();
        
        if (bit) {
            current = current->right.get();
        } else {
            current = current->left.get();
        }
        
        if (!current->left && !current->right) {
            buffer[bufferPos++] = current->ch;
            
            if (bufferPos >= bufferSize) {
                out.write(buffer, bufferSize);
                bufferPos = 0;
            }
            
            current = root.get();
        }
    }
    
    if (bufferPos > 0) {
        out.write(buffer, bufferPos);
    }
    
    in.close();
    out.close();
    
    std::cout << "decompressed: " << inputFile << " -> " << outputFile << std::endl;
}

