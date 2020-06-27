#ifndef ENCODE_H
#define ENCODE_H

#define MAX_BUFFER_SIZE 1024
#define CHARSET_SIZE 256

typedef struct Node
{
    struct Node *left;
    struct Node *right;
    int count;
    char c;
} Node;

#endif

void enqueue(Node *queue[], int *queue_length, Node *node);
Node *dequeue(Node *queue[], int *queue_length);
void get_char_count(char* f_name, int* char_count);
void make_huffman_tree(Node** root, int* char_count);
void get_encoding_map(Node *node, int bit_array[], int array_len, char *encoding[]);
void print_huffman_encoding(char* f_name, char *encoding[]);
