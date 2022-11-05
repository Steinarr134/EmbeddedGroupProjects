#include <timer_usec.h>
#include <avr/interrupt.h>

Timer_usec::Timer_usec(){}

void Timer_usec::init(){
    TCCR1A = 0;                          // set timer1 to normal operation (all bits in control registers A and B set to zero)
    TCCR1B = 0;                          //
    TCNT1 = 0;                           //  initialize counter value to 0
    //TIMSK1 |= (1 << TOIE1);              // this helps with the ovf, no Idea why 
    //sei();                             // enable interrupts

    OCR1A = 0xffff; // assign target count to compare register A (must be less than 65536)
    TCCR1B |= (1 << WGM12);      // clear the timer on compare match A
    TIMSK1 |= (1 << OCIE1A);     // set interrupt on compare match A
    TCCR1B |= (1 << CS11);               // set prescaler to 8 and start the timer
}

uint16_t Timer_usec::get() {
    if (overflow()) return 0;
    return TCNT1;
}

void Timer_usec::reset() {
    TCNT1 = 0;
}


bool Timer_usec::overflow() {
    bool tmp = (TIFR1 & (1<<TOV1));
    if(tmp){
        TIFR1 |= TOV1; // clear overflow flag, yes it's supposed to be 1 to clear: "Alternatively, TOV1 can be
         //cleared by writing a logic one to its bit location."
    }
    return tmp;
}
