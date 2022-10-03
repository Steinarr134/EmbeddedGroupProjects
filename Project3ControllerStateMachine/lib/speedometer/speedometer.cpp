#include <avr/io.h>
#include<speedometer.h>

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
