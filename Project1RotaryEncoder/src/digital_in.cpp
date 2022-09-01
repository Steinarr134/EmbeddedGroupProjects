#include "digital_in.h"
#include <avr/io.h>

Digital_in::Digital_in()
{
}


void Digital_in::init(int pin)
{
    // set DDR bit to 0
    init(pin, false);
}

void Digital_in::init(int pin, bool pull_up)
{
    pinMask = (1 << pin);
    DDRB &= ~pinMask; // DDR bit to 0
    if (pull_up)
    {
        PORTB |= pinMask; // PORT bit to 1 for pull up
    }
    else
    {
        PORTB &= ~pinMask;
    }
    // set DDR bit to 0
}

bool Digital_in::is_hi()
{
    return (PINB & pinMask);
}

bool Digital_in::is_lo()
{
    return !is_hi();
}
