#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void divideText(const char *filename, char **part1, char **part2, char **part3) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(fileSize + 1);
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);

    long partSize = fileSize / 3;
    long part1End = partSize;
    long part2End = 2 * partSize;

    // Ajustar part1End para no cortar palabras
    while (part1End < fileSize && !isspace(buffer[part1End])) {
        part1End++;
    }

    // Ajustar part2End para no cortar palabras
    while (part2End < fileSize && !isspace(buffer[part2End])) {
        part2End++;
    }

    *part1 = malloc(part1End + 1);
    strncpy(*part1, buffer, part1End);
    (*part1)[part1End] = '\0';

    *part2 = malloc(part2End - part1End + 1);
    strncpy(*part2, buffer + part1End, part2End - part1End);
    (*part2)[part2End - part1End] = '\0';

    *part3 = malloc(fileSize - part2End + 1);
    strncpy(*part3, buffer + part2End, fileSize - part2End);
    (*part3)[fileSize - part2End] = '\0';

    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    char *part1, *part2, *part3;
    divideText(argv[1], &part1, &part2, &part3);

    printf("Parte 1:\n%s\n", part1);
    printf("Parte 2:\n%s\n", part2);
    printf("Parte 3:\n%s\n", part3);

    free(part1);
    free(part2);
    free(part3);

    return 0;
}