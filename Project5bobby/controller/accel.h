#if !defined(ACCEL_H)
#define ACCEL_H
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
extern "C" {
#include <linux/i2c-dev.h>
#include </usr/include/i2c/smbus.h>
}

class Accelerometer {
    public:
    uint8_t init();
    int16_t read_x_axis();
    int16_t read_y_axis();
    int16_t read_z_axis();
    // constructor
    Accelerometer(uint8_t _accelerometer_addr, int _file) {
        accelerometer_address = _accelerometer_addr;
        file = _file;
    }
    private:
    // FXOS8700CQ internal register addresses
    uint8_t FXOS8700CQ_STATUS = 0x00;
    uint8_t FXOS8700CQ_WHOAMI = 0x0D;
    uint8_t FXOS8700CQ_XYZ_DATA_CFG = 0x0E;
    uint8_t FXOS8700CQ_CTRL_REG1 = 0x2A;
    uint8_t FXOS8700CQ_M_CTRL_REG1 = 0x5B;
    uint8_t FXOS8700CQ_M_CTRL_REG2 = 0x5C;
    uint8_t FXOS8700CQ_WHOAMI_VAL = 0xC7;
    uint8_t X_reg_msb = 0x01;
    uint8_t Y_reg_msb = 0x03;
    uint8_t Z_reg_msb = 0x05;
    uint8_t accelerometer_address;
    int file;
};

uint8_t Accelerometer::init() {
	//here we check if it is indeed the right part
	uint8_t ret;
	ret = i2c_smbus_read_byte_data(file, FXOS8700CQ_WHOAMI);
	//printf("write_byte: %d\n", ret);
	//ret = i2c_smbus_read_byte(file);
	if(ret != FXOS8700CQ_WHOAMI_VAL) {
        return ret;
	}
	uint8_t ctrl_reg;
	ctrl_reg = /*(1<<1) | */(2<<3) | (1<<2) | (1<<0);  
	ret = i2c_smbus_write_byte_data(file, FXOS8700CQ_CTRL_REG1, ctrl_reg);
    return ret; // 0 is success

}
int16_t Accelerometer::read_x_axis() {
	return (int16_t)i2c_smbus_read_byte_data(file, X_reg_msb) << 8 | i2c_smbus_read_byte_data(file, X_reg_msb+1) >>2;
}

int16_t Accelerometer::read_y_axis() {

	return (int16_t)i2c_smbus_read_byte_data(file, Y_reg_msb) << 8 | i2c_smbus_read_byte_data(file, Y_reg_msb+1) >>2;
}
int16_t Accelerometer::read_z_axis() {
	return (int16_t)i2c_smbus_read_byte_data(file, Z_reg_msb) << 8 | i2c_smbus_read_byte_data(file, Z_reg_msb+1) >>2;
}
#endif // ACCEL_H

