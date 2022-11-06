#include <stdint.h>

class Digital_in
{
public:
    Digital_in();
    void init();
    void init(int pin);
    void init(int pin, bool pull_up);
    bool is_hi();
    bool is_lo();
    bool state();

private:
    uint8_t pinMask;
};
