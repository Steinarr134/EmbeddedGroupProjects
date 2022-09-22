#include "encoder_interrupt.h"
#include <avr/io.h>
#include <avr/interrupt.h>

Encoder_interrupt::Encoder_interrupt() {}

void Encoder_interrupt::init(){
    set_interrupt_d1();
    set_interrupt_d2();
}

long Encoder_interrupt::position()
{
    return counter;
}

void Encoder_interrupt::reset(){
    counter = 0;
}

void Encoder_interrupt::pin1()
{

    // phase A
    last_state1 = PIND & (1 << PIND2);

    if (PIND & (1 << PIND2))
    {
        if (last_state2)
        {
            counter--;
        }
        else
        {
            counter++;
        }
    }
    else
    {
        if (last_state2)
        {
            counter++;
        }
        else
        {
            counter--;
        }
    }
    PORTB ^= (1 << 5);
}

void Encoder_interrupt::pin2()
{

    // phase B
    last_state2 = PIND & (1 << PIND3);
    if (PIND & (1 << PIND3))
    {
        if (last_state1)
        {
            counter++;
        }
        else
        {
            counter--;
        }
    }
    else
    {
        if (last_state1)
        {
            counter--;
        }
        else
        {
            counter++;
        }
    }
    PORTB ^= (1 << 5);
}

    void
    Encoder_interrupt::set_interrupt_d2()
{
    DDRD &= ~(1 << DDD3);
    PORTD |= (1 << PORTD3);
    last_state2 = PIND & (1 << PIND3);
    EICRA |= (1 << ISC00); // set INT0 to trigger on ANY logic change
    EIMSK |= (1 << INT0);  // Turns on INT0
    sei();                 // turn on interrupts
}
void Encoder_interrupt::set_interrupt_d1()
{
    DDRD &= ~(1 << DDD2); // set the PD2 pin as input
    PORTD |= (1 << PORTD2);
    last_state1 = PIND & (1 << PIND2);
    EICRA |= (1 << ISC10); // set INT1 to trigger on ANY logic change
    EIMSK |= (1 << INT1);  // Turns on INT1
    sei();                 // turn on interrupts
}
