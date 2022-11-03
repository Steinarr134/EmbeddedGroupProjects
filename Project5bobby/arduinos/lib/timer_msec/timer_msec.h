
class Timer_msec
{
public:
    Timer_msec();
    void init();
    void init(int period_ms);
    void init(int period_ms, float duty_cycle);
    void set(float duty_cycle);
};
