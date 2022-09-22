#include <PWM2.h>
#include <avr/interrupt.h>

PWM2::PWM2(){}

void PWM2::init(){
    DDRB |= (1<<DDB3);                   // set OC2A as output, PORTB3
    TCCR2A = (1<<COM2A0)|(1<<COM2A1)|(1<<WGM20);     // COM2A0: WGM22 = 1: Toggle OC2A on compare match.
                                         // WGM20: PWM phase correct
    TCCR2B = 0; //(1<<WGM22);                          //
    TCNT2 = 0;                           //  initialize counter value to 0, unneeded I guess
    OCR2A = 0;                         // assign target count to compare register A, start at 0% duty I guess?
    TIMSK2 = 0;                         // No interrupts
    TCCR2B |= (1 << CS21)|(1<<CS20);                // set prescaler to 8 and start the PWM generator
}

void PWM2::set(uint8_t duty_cycle){
     OCR2A = 255-duty_cycle;
}