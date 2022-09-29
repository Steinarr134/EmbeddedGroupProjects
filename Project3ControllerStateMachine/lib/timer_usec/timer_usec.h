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
