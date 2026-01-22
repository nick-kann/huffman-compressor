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

**1. Large English Text File (~24 MB)**
*Repeated sentences ("The quick brown fox...")*

**Compression:**
- Input size: 24.12 MB
- Output size: 13.94 MB
- **Ratio: 0.578 (42.2% reduction)**
- Time: 587 ms

**Decompression:**
- Input size: 13.94 MB
- Output size: 24.12 MB
- Time: 141 ms

**2. Huge CSV Data File (~100 MB)**
*Repeated structured data (numbers, dates, IDs)*

**Compression:**
- Input size: 99.65 MB
- Output size: 48.17 MB
- **Ratio: 0.483 (51.7% reduction)**
- Time: 2,785 ms (~2.8 s)

**Decompression:**
- Input size: 48.17 MB
- Output size: 99.65 MB
- Time: 725 ms

## What it does

- Reads the file and counts how often each byte shows up  
- Builds a Huffman tree and gives shorter bit codes to common bytes  
- Writes out a `.huf` file that stores the tree + compressed bits  
- Uses that same tree to rebuild the original file exactly

It just treats everything as raw bytes, so you can throw pretty much any file at it
(text, logs, whatever) as long as you decompress the `.huf` with this tool.