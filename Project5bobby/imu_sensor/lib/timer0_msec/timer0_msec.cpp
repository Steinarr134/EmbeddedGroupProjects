#include <timer0_msec.h>
#include <avr/interrupt.h>

Timer0_msec::Timer0_msec(){}

void Timer0_msec::init(){
    TCCR0A = 0;                          // set timer1 to normal operation (all bits in control registers A and B set to zero)
    TCCR0B = 0;                          //
    TCNT0 = 0;                           //  initialize counter value to 0
    OCR0A = 16000000 / 1024 - 1;         // assign target count to compare register A (must be less than 65536)
    TCCR0B |= (1 << WGM12);              // clear the timer on compare match A
    TIMSK0 |= (1 << OCIE1A);             // set interrupt on compare match A
    TCCR0B |= (1 << CS12) | (1 << CS10); // set prescaler to 1024 and start the timer
    sei();                               // enable interrupts
}

void Timer0_msec::init(int period_ms){
    // could be:
    //init(period_ms, 0.5);
    // instead, the solution to part 1:

    // period_ms has to be 4194 or smaller since  (2**16 -1)*1.024/16.000.000 =
    int max_period_ms = 255;
    if (period_ms > max_period_ms)
    {
        period_ms = max_period_ms;
    }
    // total amount of clock pulses (also convert ms to seconds)
    uint32_t total = (uint32_t)16000 *period_ms;
    // target after taking prescaler into account
    uint8_t target = total / 1024 - 1;

    // counting 16000000/1024 cycles of a clock prescaled by 1024
    TCCR0A = 0;                          // set timer1 to normal operation (all bits in control registers A and B set to zero)
    TCCR0B = 0;                          //
    TCNT0 = 0;                           //  initialize counter value to 0
    OCR0A = target;                      // assign target count to compare register A (must be less than 255)
    //TCCR0B |= (1 << WGM02);              // clear the timer on compare match A
    TIMSK0=0;
    TIMSK0 |= (1 << OCIE0A);             // set interrupt on compare match A
    
    TCCR0B |= (1 << CS02) | (1 << CS00); // set prescaler to 1024 and start the timer
    sei();                               // enable interrupts
}
