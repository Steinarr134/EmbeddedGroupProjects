#ifndef TIMER_USEC_DEFINED
#define TIMER_USEC_DEFINED

#include <stdint.h>
class Timer_usec
{
public:
    Timer_usec();
    void init();
    uint16_t get();
    void reset();
    bool overflow();
};
#endif
