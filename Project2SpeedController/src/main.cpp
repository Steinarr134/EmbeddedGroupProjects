#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <digital_out.h>
#include <timer_msec.h>
#include <encoder_simple.h>
#include <hackySerial.h>
#include <P_controller.h>

#define DELTA_T (int32_t)100 // 1/0.01 = 100
#define PPR (int32_t)(7*2*2) // 7 pulses per phase and triggering on falling as well
#define GEAR_REDUCTION (int32_t)101

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





Digital_out led(5);
Timer_msec timer;
volatile int32_t counter = 0;
volatile int32_t delta_counts = 0;
int32_t rpm = 0; // initialize just cause
ISR(TIMER1_COMPA_vect)
{
  delta_counts = counter;
  counter = 0;
}


int main()
{
  USART_Init(MYUBRR);
  timer.init(DELTA_T);
  led.init();
  
  
  set_interrupt_d1();
  set_interrupt_d2();
  uint8_t k = 100/15000;

  while (true){
    _delay_ms(100);
    delta_counts = delta_counts/10;
    rpm = delta_counts * 60 / PPR * DELTA_T / GEAR_REDUCTION;  // not accurate at all, needs fixing

    print_i(rpm);
  }

  
}

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
