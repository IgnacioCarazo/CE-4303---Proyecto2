#include <fcntl.h>    // For open()
#include <unistd.h>   // For close(), read(), write()
#include <stdio.h>    // For perror()
#include <string.h>   // For memset()

#define DEVICE_PATH "/dev/arduino_driver"
#define WRITE_LIMIT 4 // Matches driver-defined limit

// Write data to the Arduino driver
int arduino_write(const char *data, size_t size) {
    if (size > WRITE_LIMIT) {
        fprintf(stderr, "Error: Write size exceeds the limit of %d bytes.\n", WRITE_LIMIT);
        return -1;
    }

    int fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open the device for writing");
        return -1;
    }

    ssize_t bytes_written = write(fd, data, size);
    if (bytes_written < 0) {
        perror("Failed to write to the device");
        close(fd);
        return -1;
    }

    close(fd);
    return bytes_written; // Return the number of bytes written
}

// Read data from the Arduino driver
int arduino_read(char *buffer, size_t size) {
    if (size == 0) {
        fprintf(stderr, "Error: Read size must be greater than zero.\n");
        return -1;
    }

    int fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open the device for reading");
        return -1;
    }

    ssize_t bytes_read = read(fd, buffer, size - 1); // Reserve space for null terminator
    if (bytes_read < 0) {
        perror("Failed to read from the device");
        close(fd);
        return -1;
    }

    buffer[bytes_read] = '\0'; // Null-terminate the buffer for safety
    close(fd);
    return bytes_read; // Return the number of bytes read
}
