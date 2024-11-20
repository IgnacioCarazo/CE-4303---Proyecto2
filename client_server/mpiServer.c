#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>

#define PORT 8080
#define MAX_WORD_LENGTH 100
#define MAX_WORDS 10000
#define SHIFT 3 // Desplazamiento para el cifrado César

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
        if (!isdigit(token[0])) {
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

void findMostFrequentWords(WordCount *wordCounts, int numWords, WordCount *mostFrequentWords, int *count) {
    for (int i = 0; i < 15 && i < numWords; i++) {
        mostFrequentWords[i] = wordCounts[i];
    }
    *count = (numWords < 15) ? numWords : 15;
}

void descifrarCesar(char mensaje[], int shift) {
    for (int i = 0; mensaje[i]; i++) {
        if (isalpha(mensaje[i])) {
            char base = islower(mensaje[i]) ? 'a' : 'A';
            mensaje[i] = (mensaje[i] - base - shift + 26) % 26 + base;
        }
    }
}

void processBuffer(char *buffer, int shift, WordCount *mostFrequentWords, int *count, char *descifradoBuffer) {
    descifrarCesar(buffer, shift);
    strcpy(descifradoBuffer, buffer); // Copiar el buffer descifrado al buffer de salida
    WordCount wordCounts[MAX_WORDS];
    int numWords = 0;
    countWords(buffer, wordCounts, &numWords);
    findMostFrequentWords(wordCounts, numWords, mostFrequentWords, count);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char buffer[1024];
    char *fileBuffer = NULL;
    long fileSize;
    FILE *file;

    if (rank == 0) {
        // Este código solo se ejecuta en el proceso principal (nodo master)
        // Abre el archivo y lo lee en el buffer
        file = fopen("guardado.txt", "r");
        if (file == NULL) {
            perror("Error al abrir el archivo guardado.txt");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        fileBuffer = malloc(fileSize + 1);
        fread(fileBuffer, 1, fileSize, file);
        fileBuffer[fileSize] = '\0';
        fclose(file);

        // Dividir el texto en tantas partes como nodos de procesamiento
        long partSize = fileSize / size;
        for (int i = 1; i < size; i++) {
            MPI_Send(fileBuffer + i * partSize, partSize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        // Procesa la primera parte en el nodo master
        WordCount mostFrequentWords[MAX_WORDS];
        int count;
        char descifradoBuffer[partSize + 1];
        processBuffer(fileBuffer, SHIFT, mostFrequentWords, &count, descifradoBuffer);

        // Recibe los resultados de los otros nodos
        WordCount allMostFrequentWords[MAX_WORDS * size];
        int totalCount = count;
        memcpy(allMostFrequentWords, mostFrequentWords, count * sizeof(WordCount));

        for (int i = 1; i < size; i++) {
            MPI_Recv(&allMostFrequentWords[totalCount], MAX_WORDS, MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            totalCount += count;
        }

        // Procesar las palabras más frecuentes
        WordCount finalMostFrequentWords[15];
        findMostFrequentWords(allMostFrequentWords, totalCount, finalMostFrequentWords, &count);

        printf("Las palabras más frecuentes son:\n");
        for (int i = 0; i < count; i++) {
            printf("%s: %d\n", finalMostFrequentWords[i].word, finalMostFrequentWords[i].count);
        }
    } else {
        // En otros nodos, recibir los buffers, procesarlos y enviar los resultados al master
        long partSize = fileSize / size;
        MPI_Recv(buffer, partSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        WordCount mostFrequentWords[MAX_WORDS];
        int count;
        char descifradoBuffer[partSize + 1];
        processBuffer(buffer, SHIFT, mostFrequentWords, &count, descifradoBuffer);

        // Enviar el resultado de vuelta al nodo master
        MPI_Send(mostFrequentWords, count, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }

    free(fileBuffer);
    MPI_Finalize();
    return 0;
}
