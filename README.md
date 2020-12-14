# huffman_coding
Optimized Huffman encoder and decoder implementations for SENG 440.


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
Tests currently exist for character count, encoding map, and end-to-end encoding & decoding.

### Character Count / Encoding Map / Huffman Encoding
In the `main` function in `encode.c`, ensure only _one_ of the desired tests is uncommented, then recompile `encode.c` and run the corresponding test script.

For example to `test_char_count()`

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


### Huffman Decoding
From inside the `/test` directory, run
```
./test_decode.sh  
```
