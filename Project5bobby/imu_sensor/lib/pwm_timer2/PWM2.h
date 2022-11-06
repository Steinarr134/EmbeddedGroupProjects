#include <avr/io.h>
class PWM2
{
public:
    PWM2();
    void init();
    void set(uint8_t duty_cycle);
};
