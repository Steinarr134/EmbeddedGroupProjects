#include <timer_usec.h>
#include <avr/interrupt.h>

Timer_usec::Timer_usec(){}

void Timer_usec::init(){
    TCCR1A = 0;                          // set timer1 to normal operation (all bits in control registers A and B set to zero)
    TCCR1B = 0;                          //
    TCNT1 = 0;                           //  initialize counter value to 0
    TCCR1B |= (1 << CS11);               // set prescaler to 8 and start the timer
    //sei();                             // enable interrupts
}

uint16_t Timer_usec::get() {
    return TCNT1;
}

void Timer_usec::reset() {
    TCNT1 = 0;
}


bool Timer_usec::overflow() {
    bool tmp = (TIFR1 & TOV1);
    if(tmp){
        TIFR1 |= TOV1; // clear overflow flag, yes it's supposed to be 1 to clear: "Alternatively, TOV1 can be
         //cleared by writing a logic one to its bit location."
    }
    return tmp;
}
