# huffman-compressor

simple file compressor using huffman coding

## Build

```bash
make
```

## Usage

compress a file:
```bash
./huffman -c input.txt output.huf
```

decompress a file:
```bash
./huffman -d output.huf restored.txt
```

or use long flags:
```bash
./huffman --compress input.txt output.huf
./huffman --decompress output.huf restored.txt
```

show compression statistics:
```bash
./huffman -c --stats input.txt output.huf
./huffman -d --stats output.huf restored.txt
```

## Benchmarks

tested on a 22.65MB highly compressible text file (repeated patterns):

**compression:**
- input size: 22,650,000 bytes (~22.65 MB)
- output size: 14,268,879 bytes (~14.27 MB)
- ratio: 0.630 (37.0% compression)
- time: 664 ms

**decompression:**
- input size: 14,268,879 bytes (~14.27 MB)
- output size: 22,650,000 bytes (~22.65 MB)
- time: 131 ms

## What it does

- Reads the file and counts how often each byte shows up  
- Builds a Huffman tree and gives shorter bit codes to common bytes  
- Writes out a `.huf` file that stores the tree + compressed bits  
- Uses that same tree to rebuild the original file exactly

It just treats everything as raw bytes, so you can throw pretty much any file at it
(text, logs, whatever) as long as you decompress the `.huf` with this tool.