#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024
#define CHARSET_SIZE 256

// Heres what I had in mind, doesn't effect any code I've written so feel free to change
struct Node
{
    struct Node *left;
    struct Node *right;
    int count;
    char c;
};

void enqueue(struct Node *queue[], int *queue_length, struct Node *node)
{
    queue[*queue_length] = node;
    (*queue_length)++;
}

struct Node *dequeue(struct Node *queue[], int *queue_length)
{
    int min = 0;
    int i = 0;
    for (i = 0; i < *queue_length; i++)
    {
        if (queue[i]->count < queue[min]->count)
        {
            min = i;
        }
    }
    struct Node *min_node = queue[min];

    queue[min] = queue[*queue_length - 1];
    (*queue_length)--;
    return min_node;
}

void get_encoding(struct Node *node, int encoding_arr[], int encoding_len)
{
    if (node->c)
    {
        printf("'%c'(%d): ", node->c, node->count);
        int i = 0;
        while (i < encoding_len)
        {
            printf("%d", encoding_arr[i]);
            i++;
        }
        printf("\n");
        return;
    }
    if (node->left)
    {
        encoding_arr[encoding_len] = 0;
        get_encoding(node->left, encoding_arr, encoding_len + 1);
    }

    if (node->right)
    {
        encoding_arr[encoding_len] = 1;
        get_encoding(node->right, encoding_arr, encoding_len + 1);
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

    struct Node *priority_queue[CHARSET_SIZE];

    int i = 0;
    int queue_length = 0;
    while (i < CHARSET_SIZE)
    {
        if (char_count[i] > 0)
        {
            priority_queue[queue_length] = (struct Node *)malloc(sizeof(struct Node));
            priority_queue[queue_length]->count = char_count[i];
            priority_queue[queue_length]->c = i;
            priority_queue[queue_length]->left = 0;
            priority_queue[queue_length]->right = 0;
            queue_length++;
        }
        i++;
    }

    while (queue_length > 1)
    {
        struct Node *left_node = dequeue(priority_queue, &queue_length);
        struct Node *right_node = dequeue(priority_queue, &queue_length);
        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        new_node->count = left_node->count + right_node->count;
        new_node->left = left_node;
        new_node->right = right_node;
        enqueue(priority_queue, &queue_length, new_node);
    }

    struct Node *root = dequeue(priority_queue, &queue_length);
    int arr[CHARSET_SIZE] = {};
    get_encoding(root, arr, 0);
}
