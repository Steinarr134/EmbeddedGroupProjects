#ifndef HACKY_SERIAL_DEFINED
#define HACKY_SERIAL_DEFINED

#include <avr/io.h>
// for printing to serial::
// USART code taken from datasheet for printing to serial
#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1

// ints to store last state of A and B
volatile uint8_t last_stateA;
volatile uint8_t last_stateB;


void USART_Init(unsigned int ubrr)
{
  /*Set baud rate */
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  // Enable receiver and transmitter * /
      UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  /* Set frame format: 8data, 2stop bit */
  UCSR0C = (3 << UCSZ00);
}

void USART_Transmit(unsigned char data)
{
  /* Wait for empty transmit buffer */
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  /* Put data into buffer, sends the data */
  UDR0 = data;
}

bool USART_receive_ready()
{
  return (UCSR0A & (1 << RXC0));
}

unsigned char USART_Receive(void)
{
  /* Wait for data to be received */
  while (!USART_receive_ready())
    ;
  /* Get and return received data from buffer */
  return UDR0;
}

void print_one(unsigned char b){
  USART_Transmit(b);
}


void println(){
  USART_Transmit((unsigned char)10);
}

void print(unsigned char arr[], int size)
{
  for (int i = 0; i < size; i++)
  {
    USART_Transmit(arr[i]);
  }
}

void println(unsigned char arr[], int size)
{
  print(arr, size);
  println();
}

// helper function to print an integer
void print_i(int32_t i)
{
  // speial case for when i is 0
  if (i == 0){
    USART_Transmit('0');
    //USART_Transmit((unsigned char)10);
    return;
  }
  // if negative print minus sign and then just as if it were positive
  if (i < 0){
    USART_Transmit('-');
    i = -i;
  }
  // don't print leading zeros, however,do print zeros that are not leading
  bool leading_zeros_done = false;
  int m = 10000;
  for (int j = 4; j>= 0;j--) // supports 5 digit numbers (ints can't be larger)
  {
    if (leading_zeros_done || i/m > 0){
      USART_Transmit((unsigned char)(((i/m)%10) + 48));
      leading_zeros_done = true;
    }
    m /= 10;
  }
}

void print_3_numbers(int32_t a, int32_t b, int32_t c)
{
  print_i(a);
  print_one('\t');
  print_i(b);
  print_one('\t');
  print_i(c);
  print_one('\n');
}
void print_i_ln(int32_t i)
{
  print_i(i);
  println();
}
#endif
