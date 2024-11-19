#include <linux/module.h>
#include <linux/init.h>

static int __init my_init(void){
	printk("hello - Hello Kernel, this is nacho! This works man!!!\n");
	return 0;
}

static void __exit my_exit(void){
printk("hello - Goodbye, kernel! Nacho verified this worked hehe\n");
}


module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ignacio Carazo Nieto");
MODULE_DESCRIPTION("This is a smiple Hello World Kernel Module");
