#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <digital_out.h>
#include <timer_msec.h>
#include <encoder_simple.h>
// #include <digital_in.h>


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

// helper function to print an integer
void print_i(int i)
{
  // speial case for when i is 0
  if (i == 0){
    USART_Transmit('0');
    USART_Transmit((unsigned char)10);
    return;
  }
  // if negative print minus sign and then just as if it were positive
  if (i < 0){
    USART_Transmit('-');
    i = -i;
  }
  // don't print leading zeros, however,do print zeros that are not leading
  bool leading_zeros_done = false;
  for (int j = 4; j>= 0;j--) // supports 5 digit numbers (ints can't be larger)
  {
    int m = pow(10, j);
    if (leading_zeros_done || i/m > 0){
      USART_Transmit((unsigned char)(((i/m)%10) + 48));
      leading_zeros_done = true;
    }
  }
  // finally print \n 
  USART_Transmit((unsigned char)10);
}

// part 2 with interrupts

void set_interrupt_d2()
{
  DDRD &= ~(1 << DDD3);
  PORTD |= (1 << PORTD3);
  last_stateB = PIND & (1<<PIND3);
  EICRA |= (1 << ISC00);  // set INT0 to trigger on ANY logic change
  EIMSK |= (1 << INT0);   // Turns on INT0
  sei();                  // turn on interrupts
}
void set_interrupt_d1()
{
  DDRD &= ~(1 << DDD2);   // set the PD2 pin as input
  PORTD |= (1 << PORTD2);
  last_stateA = PIND & (1<<PIND2);
  EICRA |= (1 << ISC10);  // set INT1 to trigger on ANY logic change
  EIMSK |= (1 << INT1);   // Turns on INT1
  sei();                  // turn on interrupts
}


// for part 1
uint32_t counter = 0; // counts the pulses
volatile bool time_to_print = false; // printing flag
Timer_msec timer;  // timer to print every now and then
Encoder_simple encoder; // create the encoder

ISR(TIMER1_COMPA_vect)
{
  time_to_print = true;
  // led.toggle();
}
// Part 1


Digital_out led(5);


int main()
{
  bool part1 = false;
  USART_Init(MYUBRR);
  timer.init(50);
  led.init();
  // input.init(1,true);

  if (part1)
  {
    encoder.init();
    bool last = false;
    int c = 0;
    while (true){
      encoder.monitor();
      if (time_to_print)
      {
        time_to_print = false;
        print_i(encoder.position());
        // print_i(c);
        //led.toggle();

      }
    }
  }
  else
  {
    
    
    set_interrupt_d1();
    set_interrupt_d2();

    while (true){
    _delay_ms(100);
    USART_Transmit('#');
    print_i(counter);
    }

  }
}

// only counts up. never down
ISR(INT0_vect)
{
  // phase A
  last_stateA = PIND & (1<<PIND2);

  if(PIND & (1<<PIND2)) {
    if(last_stateB) {
      counter--;
    }
    else {
      counter++;
    }
  }
  else {
    if(last_stateB) {
      counter++;
    }
    else {
      counter--;
    }
  }
  PORTB ^= (1<<5);
  
}

ISR(INT1_vect)
{
    // phase B
  last_stateB = PIND & (1<<PIND3);
  if(PIND & (1<<PIND3)) {
    if(last_stateA) {
      counter++;
    }
    else {
      counter--;
    }
  }
  else {
    if(last_stateA) {
      counter--;
    }
    else {
      counter++;
    }
  }
  PORTB ^= (1<<5);
}