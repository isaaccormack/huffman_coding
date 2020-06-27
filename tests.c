#include <stdio.h>
#include <stdlib.h>
#include "encode.h"

// to generate output for test_char_count.sh
void test_char_count(int char_count[])
{
    int i = 0;
    int total = 0;
    int unique = 0;
    while (i < CHARSET_SIZE)
    {
        if (char_count[i] != 0)
        {
            unique++;
            total += char_count[i];
            if (i == 10)
            { // special case to escape line feed into correct format
                printf("Code: %5d 0x%X 'LF' Count: %d\n", i, i, char_count[i]);
            }
            else if (i == 26)
            { // special case to escape EOF into correct format
                printf("Code: %5d 0x%X 'EOF' Count: %d\n", i, i, char_count[i]);
            }
            else
            {
                printf("Code: %5d 0x%X '%c' Count: %d\n", i, i, (char)i, char_count[i]);
            }
        }
        i++;
    }
    // printf("-----TOTAL UNIQUE: %d\n", unique);
    printf("-----TOTAL CHARACTERS: %d\n", total);
}

// to generate output for test_encoding_map.sh
void test_encoding_map(char *encoding[])
{
    int i = 0;
    int unique = 0;
    while (i < CHARSET_SIZE)
    {
        if (encoding[i] != 0)
        {
            unique++;
            if (i == 10)
            { // special case to escape line feed into correct format
                printf("Code: %5d 0x%X 'LF' Encoding: %s\n", i, i, encoding[i]);
            }
            else if (i == 26)
            { // special case to escape EOF into correct format
                printf("Code: %5d 0x%X 'EOF' Encoding: %s\n", i, i, encoding[i]);
            }
            else
            {
                printf("Code: %5d 0x%X '%c' Encoding: %s\n", i, i, (char)i, encoding[i]);
            }
        }
        i++;
    }
    printf("-----TOTAL UNIQUE: %d\n", unique);
}