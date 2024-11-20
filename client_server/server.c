#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

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

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[1024] = {0};

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error al hacer bind");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 3) < 0) {
        perror("Error al escuchar");
        close(server_sock);
        return 1;
    }

    printf("Esperando conexiones...\n");

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_address_len);
        if (client_sock < 0) {
            perror("Error al aceptar la conexión");
            continue;
        }

        FILE *file = fopen("guardado.txt", "w");
        if (file == NULL) {
            perror("Error al abrir el archivo guardado.txt");
            close(client_sock);
            continue;
        }

        ssize_t bytes_read;
        int texto_recibido = 0;
        while ((bytes_read = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_read] = '\0';
            fputs(buffer, file);
            texto_recibido = 1;
        }

        if (texto_recibido) {
            printf("Texto recibido.\n");
        }

        if (bytes_read < 0) {
            perror("Error al recibir datos");
        }

        fclose(file);
        close(client_sock);

        // Leer el archivo guardado
        file = fopen("guardado.txt", "r");
        if (file == NULL) {
            perror("Error al abrir el archivo guardado.txt");
            continue;
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *fileBuffer = malloc(fileSize + 1);
        fread(fileBuffer, 1, fileSize, file);
        fileBuffer[fileSize] = '\0';
        fclose(file);

        // Dividir el texto en 3 partes
        long partSize = fileSize / 3;
        long remainder = fileSize % 3;
        char buffer1[partSize + remainder + 1], buffer2[partSize + 1], buffer3[partSize + 1];
        strncpy(buffer1, fileBuffer, partSize + remainder);
        buffer1[partSize + remainder] = '\0';
        strncpy(buffer2, fileBuffer + partSize + remainder, partSize);
        buffer2[partSize] = '\0';
        strncpy(buffer3, fileBuffer + 2 * partSize + remainder, partSize);
        buffer3[partSize] = '\0';

        // Buffers para almacenar las partes descifradas
        char descifradoBuffer1[partSize + remainder + 1], descifradoBuffer2[partSize + 1], descifradoBuffer3[partSize + 1];

        // Procesar cada parte
        WordCount mostFrequentWords1[15], mostFrequentWords2[15], mostFrequentWords3[15];
        int count1, count2, count3;

        processBuffer(buffer1, SHIFT, mostFrequentWords1, &count1, descifradoBuffer1);
        processBuffer(buffer2, SHIFT, mostFrequentWords2, &count2, descifradoBuffer2);
        processBuffer(buffer3, SHIFT, mostFrequentWords3, &count3, descifradoBuffer3);

        // Unir los buffers descifrados
        FILE *descifradoFile = fopen("descifrado.txt", "w");
        if (descifradoFile == NULL) {
            perror("Error al abrir el archivo descifrado.txt");
            continue;
        }
        fputs(descifradoBuffer1, descifradoFile);
        fputs(descifradoBuffer2, descifradoFile);
        fputs(descifradoBuffer3, descifradoFile);
        fclose(descifradoFile);

        // Unir los diccionarios y encontrar la palabra más recurrente
        WordCount combinedWords[MAX_WORDS];
        int combinedCount = 0;

        for (int i = 0; i < count1; i++) {
            combinedWords[combinedCount++] = mostFrequentWords1[i];
        }
        for (int i = 0; i < count2; i++) {
            combinedWords[combinedCount++] = mostFrequentWords2[i];
        }
        for (int i = 0; i < count3; i++) {
            combinedWords[combinedCount++] = mostFrequentWords3[i];
        }

        WordCount finalMostFrequentWords[15];
        int finalCount;
        findMostFrequentWords(combinedWords, combinedCount, finalMostFrequentWords, &finalCount);

        printf("Las palabras más frecuentes son:\n");
        for (int i = 0; i < finalCount; i++) {
            printf("%s: %d\n", finalMostFrequentWords[i].word, finalMostFrequentWords[i].count);
        }

        // Encontrar la palabra más frecuente
        WordCount mostFrequentWord = finalMostFrequentWords[0];
        for (int i = 1; i < finalCount; i++) {
            if (finalMostFrequentWords[i].count > mostFrequentWord.count) {
                mostFrequentWord = finalMostFrequentWords[i];
            }
        }

        printf("******************La palabra más frecuente es '%s' y su frecuencia es %d\n", mostFrequentWord.word, mostFrequentWord.count);

        free(fileBuffer);
    }

    close(server_sock);
    return 0;
}