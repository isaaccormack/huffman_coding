#include <stdio.h>
#include <stdlib.h>
#include "encode.h"
#include "tests.h"

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
        if (queue[i]->count < queue[min]->count ||
            (queue[i]->count == queue[min]->count &&
             ((queue[i]->c < queue[min]->c && queue[i]->c != 0) ||
              queue[min]->c == 0)))
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

// read through the file building <char,char count> map
void get_char_count(char* f_name, int* char_count) {
    FILE *fp = fopen(f_name, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Can't open %s. Exiting...\n", f_name);
        exit(1);
    }

    int c = fgetc(fp);
    if (feof(fp))
    {
        fclose(fp);
        exit(0); // file is empty so no text to encode
    }

    while (!feof(fp))
    {
        char_count[c]++;
        c = fgetc(fp);
    }

    fclose(fp);

    // add pseudo EOF, ASCII of 26 == EOF
    char_count[26]++;

}

// dynamically allocate huffman tree using a priority queue 
void make_huffman_tree(Node** root, int* char_count) {
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

    *root = dequeue(priority_queue, &queue_length);
}

// traverse through huffman tree building <char,encoding> map
void get_encoding_map(Node *node, int bit_array[], int array_len, char *encoding[])
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
        get_encoding_map(node->left, bit_array, array_len + 1, encoding);
    }

    if (node->right)
    {
        bit_array[array_len] = 1;
        get_encoding_map(node->right, bit_array, array_len + 1, encoding);
    }
    return;
}

void print_huffman_encoding(char* f_name, char *encoding[]) {

    // begin huffman message with character encodings
    for (int i = 0; i < CHARSET_SIZE; i++)
    {
        if (encoding[i] != 0)
        {
            printf("%c%s ", i, encoding[i]);
        }
    }

    // delimit character encodings with newline
    printf("\n");

    FILE *fp = fopen(f_name, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Can't open %s. Exiting...\n", f_name);
        exit(1);
    }

    // print huffman encoding by shifting bitshifting encoding into chars
    int c;
    int buf_ctr = 0;
    char char_buf = 0;
    do
    {
        c = fgetc(fp);
        if (feof(fp))
        {
            c = 26;
        }

        char *e = encoding[c];

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
            if (buf_ctr == 8)
            {
                if (putchar(char_buf) == EOF) {
                    fprintf(stderr, "Can't write stdout. Exiting...\n");
                    fclose(fp);
                    exit(1);
                } 
                buf_ctr = 0;
                char_buf = 0;
            }
        }

    } while (c != 26);

    if (ferror(fp))
    {
        fprintf(stderr, "Can't read from %s. Exiting...\n", f_name);
        fclose(fp);
        exit(1);
    }

    while (!(buf_ctr == 8 || buf_ctr == 0))
    {
        char_buf = (char_buf << 1); // shift in 0
        buf_ctr++;
    }

    if (buf_ctr != 0)
    {
        if (putchar(char_buf) == EOF) {
            fprintf(stderr, "Can't write stdout. Exiting...\n");
            fclose(fp);
            exit(1);
        } 
    }

    fclose(fp);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ./huffman_encoding input_file.txt\n");
        exit(1);
    }

    char *f_name = argv[1];

    int char_count[CHARSET_SIZE] = {0}; // [(int)char] = char count
    get_char_count(f_name, char_count);
    // test_char_count(char_count); // uncomment to test char count

    Node *root;
    make_huffman_tree(&root, char_count);

    int bit_array[CHARSET_SIZE] = {};
    char *encoding[CHARSET_SIZE] = {};
    get_encoding_map(root, bit_array, 0, encoding);
    // test_encoding_map(encoding); // uncomment to test encoding map

    print_huffman_encoding(f_name, encoding); // uncomment to print the huffman code
}
