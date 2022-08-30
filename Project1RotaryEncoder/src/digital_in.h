#include <stdint.h>

class Digital_in
{
public:
    Digital_in(int pin);
    void init();
    void init(bool pull_up);
    bool is_hi();
    bool is_lo();
    bool state();

private:
    uint8_t pinMask;
};
