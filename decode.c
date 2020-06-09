#include <stdio.h>
#include <stdlib.h>

typedef struct Entry
{
    char c;
    int code_length;
    Entry innertable[];
} Entry;

int add_table_entry(char c, char binary, int bin_length, Entry table[])
{
    return 0;
}

int main(int argc, char **argv)
{
    // Read input ---
    if (argc != 2)
    {
        printf("Usage: ./decode <file1.txt> <file2.txt> ...\n");
        exit(1);
    }

    char *f_name = argv[1];
    FILE *input_file = fopen(f_name, "rb");
    if (input_file == NULL)
    {
        fprintf(stderr, "Can't open %s. Exiting...\n", f_name);
        exit(1);
    }

    int c = fgetc(input_file);
    if (feof(input_file))
    {
        fprintf(stderr, "File is empty. Exiting...\n");
        exit(1);
    }

    Entry root_table[256];
    while (!feof(input_file))
    {
        int bin_length = 0;
        char binary_stream;

        // Adjust this to 256 bit long
        unsigned char binary = 0;
        while (1)
        {
            binary_stream = fgetc(input_file);
            if (binary_stream == '\n')
            {
                break;
            }
            if (binary_stream == ' ')
            {
                break;
            }
            printf("%c", binary_stream);
            binary |= (binary_stream == '1') << (7 - bin_length);
            bin_length++;
        }
        printf(" %c,%d\n", c, bin_length);

        if (binary_stream == '\n')
        {
            break;
        }

        // Read next char
        c = fgetc(input_file);
    }
    fclose(input_file);
    // Build table ---
    // Decode stream ---
}