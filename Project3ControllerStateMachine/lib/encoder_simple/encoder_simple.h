#ifndef ENCODER_DEFINED
#define ENCODER_DEFINED

#include <digital_in.h>

class Encoder_simple
{
public:
    Encoder_simple();
    void init();
    long position();
    void monitor();
private:
    Digital_in input1;
    Digital_in input2;
    long counter;
    bool last_state1;
    bool last_state2;
};
#endif
