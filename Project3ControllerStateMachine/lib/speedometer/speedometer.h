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

Speedo::Speedo(uint8_t i){
    interval = i;
}

void Speedo::reg(long counter){
    values[i] = counter;
    i = (i+1)%10;
}

float Speedo::speed(){
    return (values[i] - values[(i-1)%10])/(10*interval);
}

#endif
