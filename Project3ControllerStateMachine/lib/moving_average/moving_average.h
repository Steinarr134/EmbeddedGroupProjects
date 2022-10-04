#define MOVINGAVGPERIOD 10

class Moving25
{
private:
    volatile long buffer[MOVINGAVGPERIOD] = {0};
    volatile int b_i = 0;

public:
    void reset();
    void set(long val);
    double get();
};
