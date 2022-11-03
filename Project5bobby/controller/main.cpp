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

// FXOS8700CQ internal register addresses
#define FXOS8700CQ_STATUS 0x00
#define FXOS8700CQ_WHOAMI 0x0D
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E
#define FXOS8700CQ_CTRL_REG1 0x2A
#define FXOS8700CQ_M_CTRL_REG1 0x5B
#define FXOS8700CQ_M_CTRL_REG2 0x5C
#define FXOS8700CQ_WHOAMI_VAL 0xC7


int file;

int main() {
	int ACCEL_ADDR = 0x1f;
	file = open("/dev/i2c-1", O_RDWR);
	if(file <0) {
		printf("opening /dev/i2c-x failed:%d", file); //todo: check errno
		exit(1);
	}
	if(ioctl(file, I2C_SLAVE, ACCEL_ADDR) < 0) {
		printf("ioctl failed, check  for more details");
		exit(1);
	}
	
	//here we check if it is indeed the right part
	uint8_t ret;
	ret = i2c_smbus_read_byte_data(file, FXOS8700CQ_WHOAMI);
	//printf("write_byte: %d\n", ret);
	//ret = i2c_smbus_read_byte(file);
	if(ret == 0xC7) {
		printf("congrats, it's the correct chip");	
	}
	else {
		printf("wrong whoami, %d\n", ret);
		exit(1);
	}



}

