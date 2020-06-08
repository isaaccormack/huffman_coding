#!/usr/bin/env bash
gcc ../main.c

# Ensure that test_char_count() is uncommented in main.c and
# no text is printed to stdout
for i in {1..5}
do
    ./a.out infiles/test$i.txt > char_count_out/test$i.txt
    DIFF=$(diff char_count_out/test$i.txt char_count_exp/test$i.txt) 
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
