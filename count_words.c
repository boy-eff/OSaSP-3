#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>


int is_space(char ch)
{
    return (ch == ' ' || ch == '\t' || ch == '\n');
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Function %s has 2 parameters:\n", argv[0]);
        fprintf(stderr, "1: Path to file\n");
        fprintf(stderr, "2: Word to search\n");
        return -1;
    }
    char *path = argv[1];
    char *word = argv[2];
    int word_len = strlen(word);

    FILE *file;
    if ((file = fopen(path, "r")) == NULL)
    {
    perror("File opening error:");
    return -1;
    }
    
    char ch;
    int char_count = 0, word_count = 0;
    int in_word = 0;
    int i = 0;


    while ((ch = fgetc(file)) != EOF) 
    {
        char_count++;
        if (is_space(ch))
        {
            in_word = 0;
            continue;
        }

        if (in_word == 0)
        {
            in_word = 1;
            i = 0;
            while (i < word_len && ch == word[i] && ch != EOF)
            {
                ch = fgetc(file);
                char_count++;
                i++;
            }
            if (i == word_len && (is_space(ch) || ch == EOF)) word_count++;
            if (ch == EOF) break;

        }
    }
    printf("In the file %s:\n Number of bytes: %d.\n Number of words: %d.\n Process ID:%d\n", path, char_count, word_count, getpid());

    if ((fclose(file)) == EOF)
    {
    perror("File closing error:");
    return -1;
    }
      return 0;
}