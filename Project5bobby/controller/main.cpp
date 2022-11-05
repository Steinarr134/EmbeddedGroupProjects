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
#define X_reg_msb 0x01
#define Y_reg_msb 0x03
#define Z_reg_msb 0x05


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
	// todo: set range in XYZ_DATA_CFG register
	// todo: increase i2c frequency, maybe not needed if we're only reading 8 bits
	// f_Read, only 8 bits | data rate 200Hz | low noise | activate
	uint8_t ctrl_reg;
	ctrl_reg = /*(1<<1) | */(2<<3) | (1<<2) | (1<<0);  
	ret = i2c_smbus_write_byte_data(file, FXOS8700CQ_CTRL_REG1, ctrl_reg);
	
	printf("ret from write byte: %d", ret);
	printf("\n");
	int16_t z;
	while(1) {
		z = (int16_t)i2c_smbus_read_byte_data(file, Z_reg_msb) << 8 | i2c_smbus_read_byte_data(file, Z_reg_msb+1) >>2;
		printf("%d    \r", z);
		//usleep(5000L); //sleep 5ms? 
	}




}

