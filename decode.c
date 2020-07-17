#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int MAX_CHAR_ENCODING_LEN = 64;

typedef struct Entry
{
    char c;
    int code_length;
    struct Entry **innertable;
} Entry;

void print_binary(uint64_t number, int start, int length)
{
    if (start < length)
    {
        print_binary(number >> 1, start + 1, length);
        putc((number & 1) ? '1' : '0', stdout);
    }
    if(start==0){
        printf("\n");
    }
}

int add_table_entry(char c, uint64_t binary, int bin_length, Entry *table[])
{
    Entry *entry = (Entry *)malloc(sizeof(Entry));

    entry->c = c;
    entry->code_length = bin_length;

    unsigned char lower = binary >> (MAX_CHAR_ENCODING_LEN - 8);
    unsigned char upper = lower | (0b11111111 >> bin_length);

    // printf("range(%d-%d)", lower, upper);
    register int i;
    for (i = lower; i <= upper; i++)
    {
        // If encoding length is <= 8, add entries
        if (bin_length <= 8)
        {
            if (table[i] == NULL || table[i]->code_length < entry->code_length)
            {
                table[i] = entry;
            }
        }
        // If encoding length is greater than 8, add entry in nested lookup table
        else
        {
            // printf("entr_inner ");
            if (table[i] == NULL)
            {
                // If there's no existing inner table, create one and fill.
                Entry **inner_table = (Entry **)malloc(256 * sizeof(Entry *));
                entry->innertable = inner_table;
                add_table_entry(c, (binary << 8), bin_length - 8, inner_table);
                table[i] = entry;
            }
            else
            {
                // If there is an existing inner table, put entry in that table.
                add_table_entry(c, (binary << 8), bin_length - 8, table[i]->innertable);
            }
            // printf(" exit_inner ");
        }
    }
    return 0;
}

void build_lookup_table(Entry **root_table, FILE *input_file){
    int c = fgetc(input_file);
    if (feof(input_file))
    {
        fprintf(stderr, "File is empty. Exiting...\n");
        exit(1);
    }
    while (!feof(input_file))
    {
        int bin_length = 0;
        char binary_stream;

    
        // Read in one encoding mapping
        // This datatype must hold MAX_CHAR_ENCODING_LEN bits
        uint64_t binary = 0;
        while (1)
        {
            binary_stream = fgetc(input_file);
            if (binary_stream == '\n' || binary_stream == ' ')
            {
                break;
            }
            // putc(binary_stream, stdout);
            binary |= (uint64_t) (binary_stream == '1') << ((MAX_CHAR_ENCODING_LEN - 1) - bin_length);
            bin_length++;
        }

        // Add encoding mapping to table
        add_table_entry(c, binary, bin_length, root_table);
        // printf(" %c(%d)\n", c, c);

        if (binary_stream == '\n')
        {
            break;
        }

        // Read next char
        c = fgetc(input_file);
    }
}

void decode_stream(Entry **root_table, FILE *input_file){
    register unsigned char file_buffer = 0;
    register int file_buffer_length = 0;
    register int neededbits = 8;
    register unsigned char buffer = 0;
    register int shift;
    Entry **table = root_table;
    while (1)
    {
        // fill buffer from file
        while(neededbits > 0){
            shift = 8 - neededbits;
            // Read bits from file buffer
            buffer |= (file_buffer >> shift) & (0b11111111 >> shift);

            if (neededbits <= file_buffer_length)
            {
                // Remove used bits from file buffer
                file_buffer = file_buffer << (neededbits);
                file_buffer_length -= neededbits;
                neededbits = 0;
            } else {
                // If file buffer is too short, refill file buffer

                neededbits -= file_buffer_length;

                // Refresh file buffer
                file_buffer = fgetc(input_file);
                file_buffer_length = 8;
            }
        }

        // If there is no nested table, then print a character is associated with the encoding
        if (table[buffer]->innertable == NULL)
        {
            if (table[buffer]->c == 26)
            {
                fprintf(stderr, "EOT\n");
                break;
            }
            // Get code length from lookup table entry
            neededbits = table[buffer]->code_length;
            // print the character associated with the encoding
            printf("%c", table[buffer]->c);
            table = root_table;
        }
        // If there is a nested table, then set the nested table and retrieve 8 more bits
        // This means that the associated encoding is longer than 8 bits and requires a secondary table
        else
        {
            table = table[buffer]->innertable;
            neededbits = 8;
        }

        // Erase used bits
        buffer = buffer << neededbits;
    }
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

    Entry *root_table[256] = {0};
    build_lookup_table(root_table, input_file);
    decode_stream(root_table, input_file);
    fclose(input_file);
}