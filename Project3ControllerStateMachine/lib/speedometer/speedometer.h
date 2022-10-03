#ifndef SPEEDO_DEFINED
#define SPEEDO_DEFINED

#include <avr/io.h>

class Speedo
{
public:
    Speedo(uint8_t interval);
    void reg(long counter);
    float speed();


private:
    uint8_t i;
    long values[10];
    uint8_t interval;
};
#endif
