#include <stdint.h>
//#include "hackySerial.h"


class controller {
    public:
    //controller(double K_p, uint8_t max_rpm, uint8_t max_pwm, double K_i=0, double K_d=0); // why even use polymorphism
    //controller(double K_p, uint8_t max_rpm, uint8_t max_pwm);
    virtual uint8_t update(double set_point, double actual);   
    protected:
    double k_p;
    double k_i;
    double k_d;
    int16_t max_rpm_;
    uint8_t max_pwm_;
    double integral_;
    double delta_t_;
    double u;
};

class PI_controller : controller {
    public:
        PI_controller(double K_p, double K_i, uint16_t max_rpm, uint8_t max_pwm, uint8_t delta_t);
        uint8_t update(double set_point, double actual);
};


    PI_controller::PI_controller(double K_p, double K_i, uint16_t max_rpm, uint8_t max_pwm, uint8_t delta_t) {
        k_p = K_p;
        k_i = K_i;
        max_rpm_ = max_rpm;
        max_pwm_ = max_pwm;
        integral_ = 0;
        delta_t_ = (double)delta_t / 1000.0;

    }
    uint8_t PI_controller::update(double set_point, double actual) {
        double e = set_point - actual;
        //if(u <= 255 && u >= -255) { // only integrate if pwm is less than 100p 
            integral_ += e * delta_t_;
        //}
        print_i((int32_t) integral_);
        print_one('\t');
        u=k_p*(e)/max_rpm_ + k_i * integral_;
        if (u>255){ 
            return 255; 
        }
        else if (u<0) {
            return 0;
        }
        else { 
            return u*max_pwm_;
        }

        
    }