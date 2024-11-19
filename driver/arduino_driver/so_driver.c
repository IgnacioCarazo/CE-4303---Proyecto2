#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/usb.h>

// Buffer para transferencias
#define USB_BUFFER_SIZE 64
static char device_buffer[USB_BUFFER_SIZE];
static size_t buffer_size = 0;

// Tabla de dispositivos soportados
static struct usb_device_id usb_ids[] = {
    { USB_DEVICE(0x2341, 0x0043) }, // Arduino Uno
    {}
};
MODULE_DEVICE_TABLE(usb, usb_ids);

// Información del dispositivo
struct usb_arduino {
    struct usb_device *udev;
    unsigned char bulk_in_endpointAddr;
    unsigned char bulk_out_endpointAddr;
};

static struct usb_arduino *arduino_dev;

// Función de lectura desde USB
static ssize_t driver_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    int retval;
    int actual_length;

    if (!arduino_dev) {
        printk(KERN_ERR "No Arduino device connected.\n");
        return -ENODEV;
    }

    printk(KERN_INFO "Reading data from Arduino...\n");

    retval = usb_bulk_msg(arduino_dev->udev,
                          usb_rcvbulkpipe(arduino_dev->udev, arduino_dev->bulk_in_endpointAddr),
                          device_buffer, USB_BUFFER_SIZE, &actual_length, 5000);

    if (retval) {
        printk(KERN_ERR "Failed to read data from Arduino. Error %d\n", retval);
        return retval;
    }

    printk(KERN_INFO "Read %d bytes from Arduino.\n", actual_length);

    if (copy_to_user(user_buffer, device_buffer, actual_length)) {
        return -EFAULT;
    }

    return actual_length;
}

// Función de escritura hacia USB
static ssize_t driver_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset) {
    int retval;
    int actual_length;

    if (!arduino_dev) {
        printk(KERN_ERR "No Arduino device connected.\n");
        return -ENODEV;
    }

    printk(KERN_INFO "Writing %zu bytes to Arduino...\n", count);

    if (copy_from_user(device_buffer, user_buffer, count)) {
        return -EFAULT;
    }

    retval = usb_bulk_msg(arduino_dev->udev,
                          usb_sndbulkpipe(arduino_dev->udev, arduino_dev->bulk_out_endpointAddr),
                          device_buffer, count, &actual_length, 5000);

    if (retval) {
        printk(KERN_ERR "Failed to send data to Arduino. Error %d\n", retval);
        return retval;
    }

    printk(KERN_INFO "Sent %d bytes to Arduino.\n", actual_length);

    return actual_length;
}

// Función de detección del dispositivo
static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    printk(KERN_INFO "Arduino device connected: VendorID=0x%x, ProductID=0x%x\n",
           id->idVendor, id->idProduct);

    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    arduino_dev = kzalloc(sizeof(struct usb_arduino), GFP_KERNEL);
    if (!arduino_dev) {
        printk(KERN_ERR "Cannot allocate memory for Arduino device.\n");
        return -ENOMEM;
    }

    arduino_dev->udev = usb_get_dev(interface_to_usbdev(interface));
    iface_desc = interface->cur_altsetting;

    // Identificar endpoints
    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;

        printk(KERN_INFO "Endpoint %d: 0x%x, Direction: %s\n", i, endpoint->bEndpointAddress, 
               usb_endpoint_is_bulk_in(endpoint) ? "IN" : usb_endpoint_is_bulk_out(endpoint) ? "OUT" : "OTHER");

        if (usb_endpoint_is_bulk_in(endpoint)) {
            arduino_dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
            printk(KERN_INFO "Bulk IN endpoint found: 0x%x\n", arduino_dev->bulk_in_endpointAddr);
        } else if (usb_endpoint_is_bulk_out(endpoint)) {
            arduino_dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
            printk(KERN_INFO "Bulk OUT endpoint found: 0x%x\n", arduino_dev->bulk_out_endpointAddr);
        }
    }

    if (!(arduino_dev->bulk_in_endpointAddr && arduino_dev->bulk_out_endpointAddr)) {
        printk(KERN_ERR "Could not find bulk endpoints. IN: 0x%x, OUT: 0x%x\n", 
               arduino_dev->bulk_in_endpointAddr, arduino_dev->bulk_out_endpointAddr);
        kfree(arduino_dev);
        return -ENODEV;
    }

    printk(KERN_INFO "Arduino device connected successfully.\n");
    return 0;
}


// Función de desconexión del dispositivo
static void arduino_disconnect(struct usb_interface *interface) {
    if (arduino_dev) {
        usb_put_dev(arduino_dev->udev);
        kfree(arduino_dev);
        arduino_dev = NULL;
        printk(KERN_INFO "Arduino device disconnected.\n");
    }
}

static struct usb_driver arduino_driver = {
    .name = "arduino_usb_driver",
    .id_table = usb_ids,
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
};

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = driver_read,
    .write = driver_write,
};

#define MYMAJOR 80

static int __init myInitModule(void) {
    int retval;

    printk(KERN_INFO "Loading Arduino USB driver...\n");

    retval = register_chrdev(MYMAJOR, "arduino_driver", &fops);
    if (retval) {
        printk(KERN_ERR "Failed to register character device.\n");
        return retval;
    }

    retval = usb_register(&arduino_driver);
    if (retval) {
        unregister_chrdev(MYMAJOR, "arduino_driver");
        printk(KERN_ERR "Failed to register USB driver.\n");
        return retval;
    }

    printk(KERN_INFO "Arduino USB driver loaded successfully.\n");
    return 0;
}

static void __exit myExitModule(void) {
    usb_deregister(&arduino_driver);
    unregister_chrdev(MYMAJOR, "arduino_driver");
    printk(KERN_INFO "Arduino USB driver unloaded.\n");
}

module_init(myInitModule);
module_exit(myExitModule);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ignacio Carazo Nieto");
MODULE_DESCRIPTION("A specific Arduino USB driver.");
