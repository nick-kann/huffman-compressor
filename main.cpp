#include "compressor.h"
#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>
#include <iomanip>

void printUsage(const char* progName) {
    std::cout << "usage: " << progName << " [options] <input> <output>\n";
    std::cout << "\n";
    std::cout << "options:\n";
    std::cout << "  -c, --compress    compress input file\n";
    std::cout << "  -d, --decompress  decompress input file\n";
    std::cout << "  -s, --stats       show compression statistics\n";
    std::cout << "  -h, --help        show this help\n";
    std::cout << "\n";
    std::cout << "examples:\n";
    std::cout << "  " << progName << " -c file.txt file.huf\n";
    std::cout << "  " << progName << " -d file.huf file.txt\n";
    std::cout << "  " << progName << " -c --stats file.txt file.huf\n";
}

bool hasStatsFlag(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-s" || arg == "--stats") {
            return true;
        }
    }
    return false;
}

void benchmarkCompress(const std::string& inputFile, const std::string& outputFile) {
    auto start = std::chrono::high_resolution_clock::now();
    compress(inputFile, outputFile);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    try {
        size_t inputSize = std::filesystem::file_size(inputFile);
        size_t outputSize = std::filesystem::file_size(outputFile);
        double ratio = static_cast<double>(outputSize) / inputSize;
        
        std::cout << "\n--- compression stats ---\n";
        std::cout << "input size:  " << inputSize << " bytes\n";
        std::cout << "output size: " << outputSize << " bytes\n";
        std::cout << "ratio:       " << std::fixed << std::setprecision(3) << ratio << "\n";
        std::cout << "time:        " << duration.count() << " ms\n";
    } catch (const std::exception& e) {
        std::cerr << "error getting file sizes: " << e.what() << std::endl;
    }
}

void benchmarkDecompress(const std::string& inputFile, const std::string& outputFile) {
    auto start = std::chrono::high_resolution_clock::now();
    decompress(inputFile, outputFile);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    try {
        size_t inputSize = std::filesystem::file_size(inputFile);
        size_t outputSize = std::filesystem::file_size(outputFile);
        double ratio = static_cast<double>(outputSize) / inputSize;
        
        std::cout << "\n--- decompression stats ---\n";
        std::cout << "input size:  " << inputSize << " bytes\n";
        std::cout << "output size: " << outputSize << " bytes\n";
        std::cout << "ratio:       " << std::fixed << std::setprecision(3) << ratio << "\n";
        std::cout << "time:        " << duration.count() << " ms\n";
    } catch (const std::exception& e) {
        std::cerr << "error getting file sizes: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }
    
    bool useStats = hasStatsFlag(argc, argv);
    
    std::string mode;
    std::string input;
    std::string output;
    
    int argIdx = 1;
    while (argIdx < argc) {
        std::string arg = argv[argIdx];
        if (arg == "-s" || arg == "--stats") {
            argIdx++;
            continue;
        }
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        if (mode.empty()) {
            mode = arg;
        } else if (input.empty()) {
            input = arg;
        } else if (output.empty()) {
            output = arg;
        }
        argIdx++;
    }
    
    if (input.empty() || output.empty()) {
        printUsage(argv[0]);
        return 1;
    }
    
    if (mode == "-c" || mode == "--compress" || mode == "compress") {
        if (useStats) {
            benchmarkCompress(input, output);
        } else {
            compress(input, output);
        }
    } else if (mode == "-d" || mode == "--decompress" || mode == "decompress") {
        if (useStats) {
            benchmarkDecompress(input, output);
        } else {
            decompress(input, output);
        }
    } else {
        std::cerr << "invalid mode. use -c/--compress or -d/--decompress" << std::endl;
        return 1;
    }
    
    return 0;
}

