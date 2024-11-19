#ifndef ARDUINO_DRIVER_LIB_H
#define ARDUINO_DRIVER_LIB_H

#include <stddef.h> // For size_t

int arduino_write(const char *data, size_t size);
int arduino_read(char *buffer, size_t size);

#endif // ARDUINO_DRIVER_LIB_H
