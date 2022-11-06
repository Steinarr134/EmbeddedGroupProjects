/* Simple send and receive C example (line-mode terminal program with local echo)
 * for communicating with the Arduino using /dev/ttyS0. */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int file;

uint8_t address;
uint8_t function_code;
uint16_t register_number;
uint16_t register_data;

uint8_t buffer[8];

// Compute the MODBUS RTU CRC
uint16_t ModRTU_CRC(uint8_t buf[], int len)
{
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++)
    {
        //printf("crc - %02x\n", buf[pos]);
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc 
        for (int i = 8; i != 0; i--)
        { // Loop over each bit
            if ((crc & 0x0001) != 0)
            {              // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else           // Else LSB is not set
                crc >>= 1; // Just shift right
        }
    }
    //return crc;
    return (crc >> 8 ) | ((crc&0xff) << 8);
}

uint8_t crc_matches()
{
    uint16_t crc = ModRTU_CRC(buffer, 6);
    return ((((crc & 0xff) << 8) | (crc >> 8)) == (uint16_t &)buffer[6]);
}

void decrypt_buffer()
{
    address = buffer[0];
    function_code = buffer[1];
    register_number = (buffer[2] << 8) | buffer[3];
    register_data = (buffer[4] << 8) | buffer[5];
}

int transmit_buffer()
{
    return write(file, &buffer, 8);
}

int read_buffer()
{
    return read(file, &buffer, 8);
}

int send_values()
{
    buffer[0] = address;
    buffer[1] = function_code;
    buffer[2] = register_number >> 8;
    buffer[3] = register_number & 0xff;
    buffer[4] = register_data >> 8;
    buffer[5] = register_data & 0xff;
    uint16_t crc = ModRTU_CRC(buffer, 6);
    buffer[6] = crc >> 8; // I think the ModRTU function handles the LSB situation for us
    buffer[7] = crc & 0xff;
    return transmit_buffer();
}

int main(int argc, char *argv[])
{
    int count;
    if (argc != 5)
    {
        printf("Invalid number of arguments, exiting!\n");
        return -2;
    }

    if ((file = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY)) < 0)
    {
        perror("UART: Failed to open the file.\n");
        return -1;
    }

    struct termios options;

    tcgetattr(file, &options);
    // set as binary
    cfmakeraw(&options);
    options.c_cflag |= B9600 | CS8 | CREAD | CLOCAL;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);

    const size_t MSG_LEN = 8;
    uint8_t msg[MSG_LEN];

    // populate the message with integer values in binary format

    address = atoi(argv[1]);
    function_code = atoi(argv[2]);
    register_number = atoi(argv[3]);
    register_data = atoi(argv[4]);

    count = send_values();

    printf("Successfully sent(%d): ", count);
    for (uint8_t i = 0; i< 8; i++){
        printf("%02x ", buffer[i]);
    }
    putchar('\n');

    count = read_buffer();
    //sleep(2);
    if (count > 0)
    {
        printf("Successfully read: ");

        for (uint8_t i = 0; i < 8; i++)
        {
            printf("%02x ", buffer[i]);
        }
        putchar('\n');
        if (crc_matches())
        {
            printf("crc matches\n");
        }
        else
        {
            printf("crc mismatch: %d \n", ModRTU_CRC(buffer, 6));
        }
    }
    else
    {
        printf("nothing to be read\n");
    }

    close(file);


    return 0;
}
