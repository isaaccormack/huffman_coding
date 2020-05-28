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
    while (i < CHARSET_SIZE)
    {
        if (char_count[i] != 0)
        {
            total += char_count[i];
            if (i == 10)
            { // special case to escape line feed into correct format
                printf("Code: %5d 0x%X 'LF' Count: %d\n", i, i, char_count[i]);
            }
            else
            {
                printf("Code: %5d 0x%X '%c' Count: %d\n", i, i, (char)i, char_count[i]);
            }
        }
        i++;
    }
    printf("-----TOTAL CHARACTERS: %d", total);
}

void test_encoding(char *encoding[])
{
    int i = 0;
    while (i < CHARSET_SIZE)
    {
        if (encoding[i] != 0)
        {
            printf("'%c': ", (char)i);
            printf("%s", encoding[i]);
            printf("\n");
        }
        i++;
    }
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

    FILE *f_ptr = fopen(f_name, "rb");
    if (f_ptr == NULL)
    {
        fprintf(stderr, "Can't open %s. Exiting...\n", f_name);
        exit(1);
    }

    // get file size:
    fseek(f_ptr, 0, SEEK_END);
    long f_size = ftell(f_ptr);
    rewind(f_ptr);

    long bytes_read = 0;

    while (bytes_read < f_size)
    {
        int bytes_to_read = MAX_BUFFER_SIZE;

        if (f_size - bytes_read < MAX_BUFFER_SIZE)
            bytes_to_read = f_size - bytes_read;

        // allocate memory to contain the whole file:
        char *buffer = (char *)malloc(sizeof(char) * bytes_to_read);
        if (buffer == NULL)
        {
            fprintf(stderr, "Couldn't allocate buffer. Exiting...\n");
            fclose(f_ptr);
            exit(1);
        }

        // copy the file into the buffer:
        size_t result = fread(buffer, 1, bytes_to_read, f_ptr);
        if (result != bytes_to_read)
        {
            fprintf(stderr, "Couldn't read from file. Exiting...\n");
            free(buffer);
            fclose(f_ptr);
            exit(1);
        }

        bytes_read += bytes_to_read;

        // update char count
        int i = 0;
        while (i < bytes_to_read)
        {
            char_count[(int)buffer[i]]++;
            i++;
        }

        free(buffer);
    }

    fclose(f_ptr);

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
}
