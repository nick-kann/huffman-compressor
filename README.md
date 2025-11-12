# huffman-compressor

simple file compressor using huffman coding

## build

```bash
make
```

## usage

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

## how it works

counts character frequencies, builds a huffman tree, and assigns shorter codes to more common characters. works on any file type.
