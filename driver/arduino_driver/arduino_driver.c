#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/serial.h>
#include <linux/usb.h>
#include <linux/serial.h>
#include <linux/kernel.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ignacio Carazo Nieto");
MODULE_DESCRIPTION("A simple custom driver for Arduino serial communication");

/* Arduino USB Device ID: Vendor and Product IDs */
#define ARDUINO_VENDOR_ID 0x2341
#define ARDUINO_PRODUCT_ID 0x0043

/* Prototypes */
static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void arduino_disconnect(struct usb_interface *interface);

/* USB device ID table */
static struct usb_device_id arduino_table[] = {
    { USB_DEVICE(ARDUINO_VENDOR_ID, ARDUINO_PRODUCT_ID) },
    { } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, arduino_table);

/* USB driver structure */
static struct usb_driver arduino_driver = {
    .name = "arduino_custom_driver",
    .id_table = arduino_table,
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
};

static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_device *dev = interface_to_usbdev(interface);
    struct usb_serial *serial;
    int result;

    printk(KERN_INFO "Arduino Custom Driver: Arduino connected (ID: %04X:%04X)\n", id->idVendor, id->idProduct);

    /* Get the serial device */
    serial = usb_get_serial_dev(dev);
    if (!serial) {
        printk(KERN_ERR "Arduino Custom Driver: Failed to get serial device\n");
        return -ENODEV;
    }

    printk(KERN_INFO "Arduino Custom Driver: Serial device %s found\n", serial->devpath);

    /* Custom logic to interact with the Arduino here */
    return 0;
}

/* Called when Arduino is disconnected */
static void arduino_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "Arduino Custom Driver: Arduino disconnected\n");

    /* Custom cleanup logic for serial device */
    /* In the case of usb_serial, cleanup is generally handled automatically */
}

/* Module initialization */
static int __init arduino_driver_init(void) {
    int result;
    printk(KERN_INFO "Arduino Custom Driver: Initializing...\n");

    /* Register the USB driver */
    result = usb_register(&arduino_driver);
    if (result) {
        printk(KERN_ERR "Arduino Custom Driver: Failed to register USB driver\n");
        return result;
    }

    return 0;
}

/* Module exit */
static void __exit arduino_driver_exit(void) {
    printk(KERN_INFO "Arduino Custom Driver: Exiting...\n");

    /* Deregister the USB driver */
    usb_deregister(&arduino_driver);
}

module_init(arduino_driver_init);
module_exit(arduino_driver_exit);
