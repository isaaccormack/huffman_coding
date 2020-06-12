#include <stdio.h>
#include <stdlib.h>

typedef struct Entry
{
    char c;
    int code_length;
    int binary;
    struct Entry **innertable;
} Entry;

void print_binary(int number, int start, int length)
{
    if (start < length)
    {
        print_binary(number >> 1, start + 1, length);
        putc((number & 1) ? '1' : '0', stdout);
    }
}

char bit_reader(int read_bits, FILE *fp, char *buffer, int *buffer_length)
{
    if (read_bits > 8)
    {
        fprintf(stderr, "problem!\n");
        return -1;
    }
    char output = 0;
    if (read_bits <= *buffer_length)
    {
        // Read bits from buffer
        output = (*buffer >> (8 - read_bits)) & (0b11111111 >> (8 - read_bits));

        // Remove read bits from buffer
        *buffer = *buffer << (read_bits);
        *buffer_length -= read_bits;

        return output;
    }
    else if (read_bits > *buffer_length)
    {
        // Read remaining bits
        output = (*buffer >> (8 - read_bits)) & (0b11111111 >> (8 - read_bits));
        int outlength = *buffer_length;

        // Refresh buffer
        *buffer = fgetc(fp);
        *buffer_length = 8;

        // Read remaining necssary bits
        output = output | bit_reader(read_bits - outlength, fp, buffer, buffer_length);
        return output;
    }
    fprintf(stderr, "err\n");
    return -1;
}

int add_table_entry(char c, unsigned int binary, int bin_length, Entry *table[])
{
    Entry *entry = (Entry *)malloc(sizeof(Entry));

    entry->c = c;
    entry->code_length = bin_length;
    entry->binary = binary;

    unsigned char lower = binary >> 24;
    unsigned char upper = lower | (0b11111111 >> bin_length);

    printf("range(%d-%d)", lower, upper);
    int i;
    for (i = lower; i <= upper; i++)
    {
        // if lenght =< 8, no existing
        // if length =< 8, is existing
        // if length > 8, no existing
        // if length > 8, no existing
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
        else
        {
            printf("entr_inner ");
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
            printf(" exit_inner ");
        }
    }
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

    Entry *root_table[256] = {0};

    while (!feof(input_file))
    {
        int bin_length = 0;
        char binary_stream;

        // Adjust this to 256 bit long
        unsigned int binary = 0;
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
            putc(binary_stream, stdout);
            binary |= (binary_stream == '1') << (31 - bin_length);
            bin_length++;
        }
        printf(" ");
        add_table_entry(c, binary, bin_length, root_table);
        printf(" %c(%d)\n", c, c);

        if (binary_stream == '\n')
        {
            break;
        }

        // Read next char
        c = fgetc(input_file);
    }

    // Print table
    int i;
    // for (i = 0; i < 256; i++)
    // {
    //     print_binary(i, 0, 32);
    //     if (root_table[i] != NULL)
    //     {
    //         if (root_table[i]->innertable != NULL)
    //         {
    //             printf(" **hi** ");
    //         }
    //         printf(" %c(%d) ", root_table[i]->c, root_table[i]->code_length);
    //         print_binary(root_table[i]->binary, 0, 32);
    //     }
    //     printf("\n");
    // }
    // printf(" %c(%d) %c", root_table[107]->c, root_table[107]->c, root_table[107]->innertable[192]->c);

    // return 0;
    // Decode stream ---
    i = 0;
    char bit_buffer = 0;
    int buffer_length = 0;
    int neededbits = 8;
    unsigned char buffer = 0;
    Entry **table = root_table;
    while (!feof(input_file))
    {
        buffer |= bit_reader(neededbits, input_file, &bit_buffer, &buffer_length);
        neededbits = table[buffer]->code_length;

        if (table[buffer]->innertable == NULL)
        {
            if (table[buffer]->c == 26)
            {
                printf("EOT\n");
                break;
            }
            printf("%c", table[buffer]->c);
            table = root_table;
        }
        else
        {
            table = table[buffer]->innertable;
            neededbits = 8;
        }

        buffer = buffer << neededbits;
        i++;
    }
    fclose(input_file);
}