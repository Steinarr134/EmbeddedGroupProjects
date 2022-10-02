#ifndef CONTROLLER_DEFINED
#define CONTROLLER_DEFINED
#include<stdint.h>

class controller {
    public:
    virtual int16_t update(double set_point, double actual);   
    protected:
    double k_p;
    double k_i;
    double k_d;
    const int16_t max_rpm_=15000;
    const int16_t max_pwm_=255;
    double integral_;
    double delta_t_;
    double u;
    double tmp_u;
};
#endif
