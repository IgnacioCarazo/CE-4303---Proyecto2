#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	int device = open("/dev/mydevice", O_RDONLY);
	if (device == -1) {
		printf("Opnening was not possible!\n");
		return -1;
	}
	printf("Openin was succesful!\n");
	return 0;
}
