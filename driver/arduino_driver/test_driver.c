#include <stdio.h>
#include <string.h>
#include "arduino_driver_lib.h" // Include the header file for the library

#define MESSAGE_SIZE 8

int main() {
    char write_data[MESSAGE_SIZE + 1];  // +1 for null terminator

    while (1) {
        // Prompt the user to input a 8-byte message
        printf("Test File - Enter a 8-byte message (or type 'exit' to quit): ");

        // Clear the input buffer
        memset(write_data, 0, sizeof(write_data));

        // Read input using fgets
        if (fgets(write_data, sizeof(write_data), stdin) == NULL) {
            perror("Test File - Error reading input");
            continue; // Handle any read errors
        }

        // Remove newline character if user presses Enter
        write_data[strcspn(write_data, "\n")] = '\0';

        // Debugging: Print the actual input length
        printf("Test File - Input length after removing newline: %zu\n", strlen(write_data));

        // If the user types "exit", break out of the loop
        if (strcmp(write_data, "exit") == 0) {
            printf("Test File - Exiting program.\n");
            break;
        }


        // Try writing to the device and check if it was successful
        int bytes_written = arduino_write(write_data, MESSAGE_SIZE);
        if (bytes_written > 0) {
            printf("Test File - Successfully wrote %d bytes to the device: %s\n", bytes_written, write_data);
        } else {
            printf("Test File - Failed to write to the device.\n");
        }
    }

    return 0;
}
