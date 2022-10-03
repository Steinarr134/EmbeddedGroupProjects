#ifndef PWM2_H
#define PWM2_H
#include <avr/io.h>
class PWM2
{
public:
    PWM2();
    void init();
    void set(uint8_t duty_cycle);
};
#endif
