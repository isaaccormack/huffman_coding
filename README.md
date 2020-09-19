# huffman_coding
Huffman encoding and decoding implementations optimized for the ARM processor for SENG 440.


## Getting Started
1. Clone the repo
```
git clone https://github.com/isaaccormack/huffman_encoding.git
```

2. Compile the encoder and decoder (-O3 for optimizations)
```
gcc encode.c -o encode -O3 && gcc decode.c -o decode -O3
```

3. Encode a file and write its encoding to a file
```
./encode <infile.txt> > encoded.out
```

4. Decode the encoded file to stdout
```
./decode encoded.out
```


## Testing
Tests for character count, encoding map, and end-to-end encoding and decoding exist in `/test`.

### Character Count
In the `main` function in `encode.c`, ensure only `test_char_count()` is uncommented:
```
int main() {
    ...
    test_char_count(char_count);
    ...
    // test_encoding_map(encoding);
    ...
    // print_huffman_encoding(f_name, encoding);
}
```

Then, from inside the `/test` directory, run
```
./test_char_count.sh  
```

### Encoding Map
In the `main` function in `encode.c`, ensure only `test_encoding_map()` is uncommented:
```
int main() {
    ...
    // test_char_count(char_count);
    ...
    test_encoding_map(encoding);
    ...
    // print_huffman_encoding(f_name, encoding);
}
```

Then, from inside the `/test` directory, run
```
./test_encoding_map.sh  
```

### Huffman Encoding
In the `main` function in `encode.c`, ensure only `print_huffman_encoding()` is uncommented:
```
int main() {
    ...
    // test_char_count(char_count);
    ...
    // test_encoding_map(encoding);
    ...
    print_huffman_encoding(f_name, encoding);
}
```

Then, from inside the `/test` directory, run
```
./test_huffman_code.sh  
```

### Huffman Decoding
From inside the `/test` directory, run
```
./test_decode.sh  
```
