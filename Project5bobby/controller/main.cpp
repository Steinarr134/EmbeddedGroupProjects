#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
extern "C" {
#include <linux/i2c-dev.h>
#include </usr/include/i2c/smbus.h>
}

#include "accel.h"



int file;
int ACCEL_ADDR = 0x1f;

int main() {
	file = open("/dev/i2c-1", O_RDWR); // possible to hide this from user? in case user wants to use i2c for more devices?
	if(file <0) {
		printf("opening /dev/i2c-x failed:%d", file); //todo: check errno
		exit(1);
	}
	if(ioctl(file, I2C_SLAVE, ACCEL_ADDR) < 0) {
		printf("ioctl failed, check  for more details");
		exit(1);
	}

	Accelerometer accel = Accelerometer(ACCEL_ADDR, file);
	uint8_t ret = accel.init();
	if(ret) {
		printf("something failed: %d\n", ret);
		exit(1);
	}
	
	int16_t x,y,z;
	while(1) {

		x = accel.read_x_axis();
		y = accel.read_y_axis();
		z = accel.read_z_axis();
		printf("x: %d, y: %d, z: %d                   \r", x, y, z);
		//usleep(5000L); //sleep 5ms? 
	}




}

