#ifndef CONTROLLER_DEFINED
#define CONTROLLER_DEFINED
#include<stdint.h>
#include <random_defs.h>

class controller {
    public:
    virtual int16_t update(double set_point, double actual);   
    int16_t get_max_rpm(){return max_rpm_;};   
    protected:
    double k_p;
    double k_i;
    double k_d;
    const int16_t max_rpm_=MAX_RPM;
    const int16_t max_pwm_=MAX_PWM;
    double integral_;
    double delta_t_;
    double u;
    double tmp_u;
};
#endif
