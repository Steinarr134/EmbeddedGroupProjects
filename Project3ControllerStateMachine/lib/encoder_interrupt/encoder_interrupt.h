
#ifndef ENCODER_INT_DEFINED
#define ENCODER_INT_DEFINED

class Encoder_interrupt
{
public:
    Encoder_interrupt();
    void init();
    long position();
    void pin1();
    void pin2();
    void reset();
    bool forward();

private:
    long counter;
    bool last_state1;
    bool last_state2;
    bool forward_; // 1 forward, 0 reverse
    void set_interrupt_d1();
    void set_interrupt_d2();
};
#endif
