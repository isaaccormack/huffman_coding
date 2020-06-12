#!/usr/bin/env bash
gcc ../encode.c -o encode.out
gcc ../decode.c -o decode.out

for i in {1..5}
do
    ./encode.out infiles/test$i.txt
    DIFF=$(./decode.out huffman_encoding_out.txt | diff ./infiles/test$i.txt -)
    if [ "$DIFF" != "" ] 
    then
        echo "TEST $i FAILED" 
        echo "$DIFF"
        echo ""
    else
        echo "TEST $i PASSED" 
    fi
done

rm encode.out decode.out