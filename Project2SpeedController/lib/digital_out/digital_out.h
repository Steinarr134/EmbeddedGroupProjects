#include <stdint.h>

class Digital_out
{
public:
    Digital_out(int pin); 
    void init();
    void set_hi();
    void set_lo();
    void toggle();
    const bool state() {return _state;}


private:
    bool _state;
    uint8_t pinMask;
};


