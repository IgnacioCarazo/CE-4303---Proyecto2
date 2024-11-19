#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8080

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
            continue; // No cerrar el servidor, seguir esperando conexiones
        }

        FILE *file = fopen("guardado.txt", "w");
        if (file == NULL) {
            perror("Error al abrir el archivo guardado.txt");
            close(client_sock);
            continue; // No cerrar el servidor, seguir esperando conexiones
        }

        ssize_t bytes_read;
        int texto_recibido = 0;
        while ((bytes_read = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_read] = '\0';
            fputs(buffer, file);
            texto_recibido = 1;
        }

        if (texto_recibido) {
            printf("Texto recibido.\n"); // Imprimir mensaje indicando que se recibió texto
        }

        if (bytes_read < 0) {
            perror("Error al recibir datos");
        }

        fclose(file);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}