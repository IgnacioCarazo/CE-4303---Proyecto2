# Kernel module configuration
obj-m += so_driver.o

# Arduino driver library and test driver configuration
LIB_NAME = libarduino_driver.a
LIB_OBJ = arduino_driver_lib.o
TEST_EXEC = test_driver
CC = gcc

# Targets
all: kernel_module arduino_driver

kernel_module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

arduino_driver: $(LIB_NAME) $(TEST_EXEC)

$(LIB_OBJ): arduino_driver_lib.c
	$(CC) -c $< -o $@

$(LIB_NAME): $(LIB_OBJ)
	ar rcs $@ $<

$(TEST_EXEC): test_driver.c $(LIB_NAME)
	$(CC) $< -L. -larduino_driver -o $@

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) $(LIB_OBJ) $(LIB_NAME) $(TEST_EXEC)
