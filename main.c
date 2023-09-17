#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_WORD_LEN 30


struct WordNode
{
    int count;
    char word[MAX_WORD_LEN + 1];
    struct WordNode *next;
};


struct WordNode *init_word_node(const char *word)
{
    struct WordNode *node = malloc(sizeof(struct WordNode));
    if (node != NULL)
    {
        node->count = 1;
        strncpy(node->word, word, MAX_WORD_LEN);
        node->word[MAX_WORD_LEN] = '\0'; // Ensure null-terminated string
        node->next = NULL;
    }
    return node;
}


struct WordNode *upsert_word(struct WordNode *head, const char *word)
{
    if (head == NULL)
    {

        return init_word_node(word);
    }

    struct WordNode *current = head;
    struct WordNode *prev = NULL;

    while (current != NULL)
    {
        if (strcmp(current->word, word) == 0)
        {

            current->count++;
            return head;
        }
        prev = current;
        current = current->next;
    }


    struct WordNode *new_node = init_word_node(word);
    if (new_node != NULL)
    {
        prev->next = new_node;
    }
    return head;
}

void free_word_list(struct WordNode *head)
{
    while (head != NULL)
    {
        struct WordNode *temp = head;
        head = head->next;
        free(temp);
    }
}


void print_words_by_count(struct WordNode *head, int count)
{
    struct WordNode *current = head;

    while (current != NULL)
    {
        if (current->count == count)
        {
            printf("- %s\n", current->word);
        }
        current = current->next;
    }
}


int process_file(const char *input_file, const char *output_file, int verbose)
{
    FILE *input = fopen(input_file, "r");
    if (input == NULL)
    {
        printf("Unable to open input file \"%s\".\n", input_file);
        return -1;
    }

    int max_count = 0;
    struct WordNode *word_list = NULL;
    char word_buf[MAX_WORD_LEN + 1] = "";
    int buf_p = 0;

    while (1)
    {
        int c = fgetc(input);

        if (!isalpha(c) && c != '-')
        {

            if (buf_p > 0)
            {
                word_buf[buf_p] = '\0';
                word_list = upsert_word(word_list, word_buf);
                max_count = (word_list->count > max_count) ? word_list->count : max_count;
                buf_p = 0;
            }

            if (c == EOF)
            {
                break; // End of file
            }
        }
        else
        {
            if (buf_p < MAX_WORD_LEN)
            {

                word_buf[buf_p++] = tolower(c);
            }
            else
            {
                while (isalpha(c) || c == '-')
                {
                    c = fgetc(input);
                }
                buf_p = 0;
            }
        }
    }

    fclose(input);

    if (verbose)
    {
        struct WordNode *current = word_list;
        printf("Word counts:\n");
        while (current != NULL)
        {
            printf("\"%s\", %d\n", current->word, current->count);
            current = current->next;
        }
    }


    printf("\nHighest count: %d\nWords with the highest count:\n", max_count);
    print_words_by_count(word_list, max_count);


    FILE *output = fopen(output_file, "w");
    if (output == NULL)
    {
        printf("Unable to open output file \"%s\" for writing.\n", output_file);
        return -1;
    }

    struct WordNode *current = word_list;
    while (current != NULL)
    {
        fprintf(output, "\"%s\", %d\n", current->word, current->count);
        current = current->next;
    }


    fclose(output);


    free_word_list(word_list);

    return 0;
}

int main()
{
    const char *input_file = "input.txt";
    const char *output_file = "output.txt";
    int verbose = 1;

    if (process_file(input_file, output_file, verbose))
    {
        printf("Oops, something went wrong.\n");
        return -1;
    }

    return 0;
}