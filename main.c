#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024
#define CHARSET_SIZE 256

// Heres what I had in mind, doesn't effect any code I've written so feel free to change
typedef struct Node
{
    struct Node *left;
    struct Node *right;
    int count;
    char c;
} Node;

// add node to end of priority queue
void enqueue(Node *queue[], int *queue_length, Node *node)
{
    queue[*queue_length] = node;
    (*queue_length)++;
}

// pop min node
Node *dequeue(Node *queue[], int *queue_length)
{
    // find index of node with lowest count and lowest ASCII value
    int min = 0;
    int i = 0;
    for (i = 0; i < *queue_length; i++)
    {
        if (queue[i]->count < queue[min]->count || (queue[i]->count == queue[min]->count && queue[i]->c < queue[min]->c))
        {
            min = i;
        }
    }
    Node *min_node = queue[min];

    // replace popped node with node at end
    queue[min] = queue[*queue_length - 1];
    (*queue_length)--;
    return min_node;
}

// traverse through huffman tree building <char,encoding> map
void get_encoding(Node *node, int bit_array[], int array_len, char *encoding[])
{
    // If a node has a char value, then it is a leaf node
    if (node->c)
    {
        encoding[node->c] = (char *)malloc(sizeof(char) * array_len);

        // convert int array into string
        int i = 0;
        char bit;
        while (i < array_len)
        {
            sprintf(&encoding[node->c][i], "%d", bit_array[i]);
            i++;
        }
        return;
    }

    if (node->left)
    {
        bit_array[array_len] = 0;
        get_encoding(node->left, bit_array, array_len + 1, encoding);
    }

    if (node->right)
    {
        bit_array[array_len] = 1;
        get_encoding(node->right, bit_array, array_len + 1, encoding);
    }
    return;
}

// used in conjunction with test_char_count.sh
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

void test_encoding(char *encoding[])
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
            else            {
                printf("Code: %5d 0x%X '%c' Encoding: %s\n", i, i, (char)i, encoding[i]);
            }
        }
        i++;
    }
    printf("-----TOTAL UNIQUE: %d\n", unique);
}

void print_expected_huffman_code(char *infile, char *encoding[])
{
    FILE* fp = fopen(infile, "r");

    int ctr = 0;
    int c = 0;
    do {
        c = fgetc(fp);
        if (feof(fp))
        {
            c = 26;
        }

        char* e = encoding[c];

        for (int i = 0; e[i] != '\0'; i++)
        {
            printf("%c", e[i]);
            ctr++;
            if (ctr == 8)
            {
                printf(" ");
                ctr = 0;
            }
        }
    } while (c != 26);

    while (!(ctr == 8 || ctr == 0))
    {
        printf("0"); // fill in the rest of the byte with 0's
        ctr++;
    }

    // space before \n to match formating in test_huffman_code for convenience
    // if ctr is 0, then there would be a double space at the end, since a space was just printed
    if (ctr != 0) {
        printf(" ");
    }
    printf("\n");

    fclose(fp);
}

void test_huffman_code(char *encoding[]) {
    FILE* huff_fp = fopen("huffman_encoding_out.txt", "r");


    // ignore character encodings on first line
    int c = 0;
    c = fgetc(huff_fp);
    while (c != '\n')
        c = fgetc(huff_fp);


    do {
        c = fgetc(huff_fp);
        if (feof(huff_fp))
        {
            break;
        }
        // shift c such that first bit of byte is msb
        c <<= 24;

        // need to write this out to huff_out.txt
        for (int i = 0; i < 8; i++)
        {
            if (c & (1 << 31)) // if msb is set
                printf("1");
            else
                printf("0");

            c <<= 1;
        }
        printf(" "); // need to fix where this prints this space

    } while (1);
    printf("\n");

    fclose(huff_fp);
}

int main(int argc, char **argv)
{
    // could support many input files and running test files if input files later
    if (argc != 2)
    {
        printf("Usage: ./huffman_encoding <file1.txt> <file2.txt> ...\n");
        exit(1);
    }

    char *f_name = argv[1];

    // index array via ascii of char, value is its count
    int char_count[CHARSET_SIZE] = {0};


    FILE *fp = fopen(f_name, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Can't open %s. Exiting...\n", f_name);
        exit(1);
    }

    int c = fgetc(fp);
    if (feof(fp)) {
        // this text to stdout is needed for char count test1.txt
        fprintf(stdout, "File is empty. Exiting...\n");
        exit(1);
    }

   while (!feof(fp)) {
        char_count[c]++;
        c = fgetc(fp);
   }

    fclose(fp);

    // add pseudo EOF, ASCII of 26 == EOF
    char_count[26]++;

    // test_char_count(char_count); // uncomment to test char count

    // add nodes to priority queue
    Node *priority_queue[CHARSET_SIZE];
    int i = 0;
    int queue_length = 0;
    while (i < CHARSET_SIZE)
    {
        if (char_count[i] > 0)
        {
            priority_queue[queue_length] = (Node *)malloc(sizeof(Node));
            priority_queue[queue_length]->count = char_count[i];
            priority_queue[queue_length]->c = i;
            priority_queue[queue_length]->left = 0;
            priority_queue[queue_length]->right = 0;
            queue_length++;
        }
        i++;
    }

    // while there is at least 2 node left in queue, loop
    while (queue_length > 1)
    {
        // pop top 2 nodes
        Node *left_node = dequeue(priority_queue, &queue_length);
        Node *right_node = dequeue(priority_queue, &queue_length);

        // create new node, set children as popped nodes
        Node *new_node = (Node *)malloc(sizeof(Node));
        new_node->count = left_node->count + right_node->count;
        new_node->left = left_node;
        new_node->right = right_node;

        // enqueue new node
        enqueue(priority_queue, &queue_length, new_node);
    }

    // last node is root
    Node *root = dequeue(priority_queue, &queue_length);
    int bit_array[CHARSET_SIZE] = {};
    char *encoding[CHARSET_SIZE] = {};

    // traverse tree, get encoding
    get_encoding(root, bit_array, 0, encoding);

    // test_encoding(encoding); // uncomment to test encoding

    // create new output file here
    FILE* out_fp = fopen("huffman_encoding_out.txt", "w+");
    if (out_fp == NULL)
    {
        fprintf(stderr, "Can't create huffman_encoding_out.txt. Exiting...\n");
        exit(1);
    }

    // write character encodings into huffman message
    for (int i = 0; i < CHARSET_SIZE; i++)
    {
        if (encoding[i] != 0)
        {
            // fprintf returns EOF on error
            if (fprintf(out_fp, "%d=%s ", i, encoding[i]) < 0)
            {
                fprintf(stderr, "Can't write to huffman_encoding_out.txt. Exiting...\n");
                exit(1);
            }
        }
    }
    // delimit character encodins with newline
    if (fprintf(out_fp, "\n") < 0)
    {
        fprintf(stderr, "Can't write to huffman_encoding_out.txt. Exiting...\n");
        exit(1);
    }

    // write huffman encoding to file by shifting bitshifting encoding into chars
    fp = fopen(f_name, "r");
    fseek(fp, 0, SEEK_SET);

    int buf_ctr = 0;
    char char_buf = 0;
    do {
        c = fgetc(fp);
        if (feof(fp))
        {
            c = 26;
        }

        char* e = encoding[c];

        for (int i = 0; e[i] != '\0'; i++)
        {
            if (e[i] == '1')
            {
                char_buf = ((char_buf << 1) | 1); // shift in 1
            }
            else // e[i] == '0'
            {
                char_buf = (char_buf << 1); // shift in 0
            }

            buf_ctr++;

            // once there is a full char to write to file
            if (buf_ctr == 8) {
                fputc(char_buf, out_fp);
                buf_ctr = 0;
                char_buf = 0;
            }
        }

    } while (c != 26);

    if (ferror(fp))
    {           
        fprintf(stderr, "Can't read from %s. Exiting...\n", f_name);
        exit(1);
    }

    while (!(buf_ctr == 8 || buf_ctr == 0)) {
        char_buf = (char_buf << 1); // shift in 0
        buf_ctr++;
    }

    if (buf_ctr != 0) {
        fputc(char_buf, out_fp);
    }

    if (ferror(out_fp))
    {           
        fprintf(stderr, "Can't write to huffman_encoding_out.txt. Exiting...\n");
        exit(1);
    }

    // could handle closing of files on error, not a big deal though
    fclose(out_fp);
    fclose(fp);

    // print_expected_huffman_code(f_name, encoding);  // uncomment to print the expected huffman code
    // test_huffman_code(encoding); // uncomment to print the huffman code generated

}
