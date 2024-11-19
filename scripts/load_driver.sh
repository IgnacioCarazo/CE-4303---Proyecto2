#!/bin/bash

# Remove the cdc_acm module if it's loaded
sudo modprobe -r cdc_acm

# Insert your custom driver module
sudo insmod ../driver/arduino_driver/so_driver.ko

# Create the device node for the driver
sudo mknod /dev/arduino_driver c 80 0

echo "Driver loaded and device node created successfully."
