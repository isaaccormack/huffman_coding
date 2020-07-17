#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int MAX_CHAR_ENCODING_LEN = 64;

typedef struct Entry
{
    char c;
    int code_length;
    uint64_t binary;
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

// This function oututs a number of bits equal to read_bits.
char bit_reader(int read_bits, FILE *fp, char *buffer, int *buffer_length)
{
    if (read_bits > 8)
    {
        fprintf(stderr, "Read bits should never be greater than 8!\n");
        exit(1);
    }
    register char output = 0;
    register int shift = 8 - read_bits;
    if (read_bits <= *buffer_length)
    {
        // Read bits from buffer
        output = (*buffer >> shift) & (0b11111111 >> shift);

        // Remove read bits from buffer
        *buffer = *buffer << (read_bits);
        *buffer_length -= read_bits;

        return output;
    }
    
    // Read remaining bits
    output = (*buffer >> shift) & (0b11111111 >> shift);
    read_bits -= *buffer_length;

    // Refresh buffer
    *buffer = fgetc(fp);
    *buffer_length = 8;

    // Read remaining necssary bits
    return output | bit_reader(read_bits, fp, buffer, buffer_length);
}

int add_table_entry(char c, uint64_t binary, int bin_length, Entry *table[])
{
    Entry *entry = (Entry *)malloc(sizeof(Entry));

    entry->c = c;
    entry->code_length = bin_length;
    entry->binary = binary;

    unsigned char lower = binary >> (MAX_CHAR_ENCODING_LEN - 8);
    unsigned char upper = lower | (0b11111111 >> bin_length);

    // printf("range(%d-%d)", lower, upper);
    int i;
    for (i = lower; i <= upper; i++)
    {
        // If encoding length is <= 8, add entries
        if (bin_length <= 8)
        {
            if (table[i] == NULL)
            {
                table[i] = entry;
            }
            else if (table[i]->code_length < entry->code_length)
            {
                table[i] = entry;
            }
        }
        // If encoding length is greater than 8, add entry in nested lookup table
        else
        {
            // printf("entr_inner ");
            if (table[i] == NULL || table[i]->innertable == NULL)
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
            if (binary_stream == '\n')
            {
                break;
            }
            if (binary_stream == ' ')
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
    char bit_buffer = 0;
    int buffer_length = 0;
    register int neededbits = 8;
    register unsigned char buffer = 0;
    Entry **table = root_table;
    while (!feof(input_file))
    {
        // fill buffer from file with
        buffer |= bit_reader(neededbits, input_file, &bit_buffer, &buffer_length);


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
    if (argc != 2)
    {
        printf("Usage: ./decode <input_file.txt>\n");
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