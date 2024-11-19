#!/bin/bash

# Remove the custom driver module
sudo rmmod so_driver

# Remove the device node for the driver
sudo rm /dev/arduino_driver

echo "Driver unloaded and device node removed successfully."
