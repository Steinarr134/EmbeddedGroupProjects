
class Moving25
{
private:
    volatile int buffer[25] = {0};
    volatile int b_i = 0;

public:
    void reset();
    void set(int val);
    double get();
};
