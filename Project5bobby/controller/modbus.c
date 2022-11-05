/* Simple send and receive C example (line-mode terminal program with local echo)
 * for communicating with the Arduino using /dev/ttyS0. */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Compute the MODBUS RTU CRC
uint16_t ModRTU_CRC(uint8_t buf[], int len)
{
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++)
    {
        printf("crc - %02x\n", buf[pos]);
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
    return (crc >> 8 ) | ((crc&0xff) << 8);
}

int main(int argc, char *argv[])
{
    int file, count;

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

    uint8_t address = atoi(argv[1]);
    uint8_t function_code = atoi(argv[2]);
    uint16_t register_number = atoi(argv[3]);
    uint16_t register_data = atoi(argv[4]);

    msg[0] = address;
    msg[1] = function_code;
    msg[2] = register_number >> 8;
    msg[3] = register_number & 0xff;
    msg[4] = register_data >> 8;
    msg[5] = register_data & 0xff;
    uint16_t crc = ModRTU_CRC(msg, 6);
    msg[6] = crc >> 8; // I think the ModRTU function handles the LSB situation for us
    msg[7] = crc & 0xff;

    msg[0] = atoi(argv[1]);
    msg[1] = atoi(argv[2]);

    // write to serial
    if (count = write(file, msg, MSG_LEN) < 0)
    {
        perror("Failed to write to the output\n");
        return -1;
    }

    printf("Successfully sent: ");
    for (uint8_t i = 0; i< MSG_LEN; i++){
        printf("%02x ", msg[i]);
    }
    putchar('\n');

    count = read(file, &msg, MSG_LEN);
    //sleep(2);
    if (count > 0)
    {
        printf("Successfully read: ");

        for (uint8_t i = 0; i < MSG_LEN; i++)
        {
            printf("%02x ", msg[i]);
        }
        putchar('\n');
    }
    else
    {
        printf("nothing to be read\n");
    }

    close(file);


    return 0;
}
