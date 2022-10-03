#ifndef HACKY_SERIAL_H_
#define HACKY_SERIAL_H_
// // for printing to serial::
// // USART code taken from datasheet for printing to serial
// #define FOSC 16000000 // Clock Speed
// #define BAUD 9600
// #define MYUBRR FOSC / 16 / BAUD - 1

// // ints to store last state of A and B
// volatile uint8_t last_stateA;
// volatile uint8_t last_stateB;


void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void print_one(unsigned char b);
void println();

// helper function to print an integer
void print_i(int32_t i);

void print_3_numbers(int32_t a, int32_t b, int32_t c);
void print_i_ln(int32_t i);
#endif
