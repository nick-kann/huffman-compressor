#include "compressor.h"
#include <iostream>
#include <string>

void printUsage(const char* progName) {
    std::cout << "usage: " << progName << " [options] <input> <output>\n";
    std::cout << "\n";
    std::cout << "options:\n";
    std::cout << "  -c, --compress    compress input file\n";
    std::cout << "  -d, --decompress  decompress input file\n";
    std::cout << "  -h, --help        show this help\n";
    std::cout << "\n";
    std::cout << "examples:\n";
    std::cout << "  " << progName << " -c file.txt file.huf\n";
    std::cout << "  " << progName << " -d file.huf file.txt\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string mode = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];
    
    if (mode == "-h" || mode == "--help") {
        printUsage(argv[0]);
        return 0;
    }
    
    if (mode == "-c" || mode == "--compress") {
        compress(input, output);
    } else if (mode == "-d" || mode == "--decompress") {
        decompress(input, output);
    } else if (mode == "compress") {
        compress(input, output);
    } else if (mode == "decompress") {
        decompress(input, output);
    } else {
        std::cerr << "invalid mode. use -c/--compress or -d/--decompress" << std::endl;
        return 1;
    }
    
    return 0;
}

