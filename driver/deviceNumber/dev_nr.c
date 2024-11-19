#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int driver_open(struct inode *device_file, struct file *instance) {
	printk("de_nr - Open was called!\n");
	return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - Close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};

#define MYMAJOR 80

static int __init myInitModule(void) {
	int retval;
	printk("dev_nr - Hello Kernel, this is nacho! This works man!!!\n");
	/* register device nr. */
	retval = register_chrdev(MYMAJOR, "my_dev_nr", &fops);
	if (retval == 0) {
		printk("dev_nr - Registered Device numnber Major: %d, Minor: %d\n", MYMAJOR, 0);
	}
	else if (retval > 0) {
		printk("dev_nr - Registered Device number Major: %d, Minor: %d\n", retval>>20, retval&0xfffff);
	}
	else {
		printk("Error - Could not register device number!\n:");
		return -1;
	}
	return 0;
}

static void __exit myExitModule(void){
	unregister_chrdev(MYMAJOR, "my_dev_nr");
	printk("dev_nr - Goodbye, kernel! Nacho verified this worked hehe\n");
}


module_init(myInitModule);
module_exit(myExitModule);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ignacio Carazo Nieto");
MODULE_DESCRIPTION("This will register a device number and implements some callback function");
