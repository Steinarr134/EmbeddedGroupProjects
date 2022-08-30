#include "digital_in.h"
#include <avr/io.h>

Digital_in::Digital_in(int pin)
{
    pinMask = (1 << pin);
}

void Digital_in::init()
{
    // set DDR bit to 0
    init(false);
}

void Digital_in::init(bool pull_up)
{
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
