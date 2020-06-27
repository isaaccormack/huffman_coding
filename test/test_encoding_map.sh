#!/usr/bin/env bash
cd ..
make
cd test
cp ../encode a.out

# Ensure that test_encoding_map() is uncommented in encode.c and
# no text is printed to stdout
for i in {1..2}
do
    ./a.out infiles/test$i.txt > encoding_map_out/test$i.txt
    DIFF=$(diff encoding_map_out/test$i.txt encoding_map_exp/test$i.txt) 
    if [ "$DIFF" != "" ] 
    then
        echo "TEST $i FAILED" 
        echo "$DIFF"
        echo ""
    else
        echo "TEST $i PASSED" 
    fi
done

rm a.out
