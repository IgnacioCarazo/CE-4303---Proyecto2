#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 100
#define MAX_WORDS 10000
#define TOP_N 15

typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordCount;

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void countWords(char *buffer, WordCount *wordCounts, int *numWords) {
    char *token = strtok(buffer, " .,:;?\n");
    while (token != NULL) {
        if (!isdigit(token[0])) { // Ignorar los tokens que comienzan con un número
            toLowerCase(token);
            int found = 0;
            for (int i = 0; i < *numWords; i++) {
                if (strcmp(wordCounts[i].word, token) == 0) {
                    wordCounts[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(wordCounts[*numWords].word, token);
                wordCounts[*numWords].count = 1;
                (*numWords)++;
            }
        }
        token = strtok(NULL, " .,:;?\n");
    }
}

int compareWordCount(const void *a, const void *b) {
    WordCount *wordCountA = (WordCount *)a;
    WordCount *wordCountB = (WordCount *)b;
    return wordCountB->count - wordCountA->count;
}

void findTopNWords(WordCount *wordCounts, int numWords, WordCount *topWords, int n) {
    qsort(wordCounts, numWords, sizeof(WordCount), compareWordCount);
    for (int i = 0; i < n && i < numWords; i++) {
        topWords[i] = wordCounts[i];
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(fileSize + 1);
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';

    fclose(file);

    WordCount wordCounts[MAX_WORDS];
    int numWords = 0;

    countWords(buffer, wordCounts, &numWords);

    WordCount topWords[TOP_N];
    findTopNWords(wordCounts, numWords, topWords, TOP_N);

    printf("Las %d palabras más frecuentes son:\n", TOP_N);
    for (int i = 0; i < TOP_N && i < numWords; i++) {
        printf("%s: %d\n", topWords[i].word, topWords[i].count);
    }

    free(buffer);

    return 0;
}