#include <stdint.h>
//#include "hackySerial.h"


class controller {
    public:
    //controller(double K_p, uint8_t max_rpm, uint8_t max_pwm, double K_i=0, double K_d=0); // why even use polymorphism
    //controller(double K_p, uint8_t max_rpm, uint8_t max_pwm);
    virtual int16_t update(double set_point, double actual);   
    protected:
    double k_p;
    double k_i;
    double k_d;
    int16_t max_rpm_;
    int16_t max_pwm_;
    double integral_;
    double delta_t_;
    double u;
};

class PI_controller : controller {
    public:
        PI_controller(double K_p, double K_i, uint16_t max_rpm, int16_t max_pwm, uint8_t delta_t);
        int16_t update(double set_point, double actual);
};


    PI_controller::PI_controller(double K_p, double K_i, uint16_t max_rpm, int16_t max_pwm, uint8_t delta_t) {
        k_p = K_p;
        k_i = K_i;
        max_rpm_ = max_rpm;
        max_pwm_ = max_pwm;
        integral_ = 0;
        delta_t_ = (double)delta_t / 1000.0;

    }
    int16_t PI_controller::update(double set_point, double actual) {
        double tmp_u;
        double e = set_point - actual;
        
        //if(u <= 255 && u >= -255) { // only integrate if pwm is less than 100p 
            integral_ += e * delta_t_;
        //}
        
        print_i((int32_t) u);
        print_one('\t');
        print_i((int32_t) integral_);
        print_one('\t');
        u=k_p*(e) + k_i * integral_;
        tmp_u = u*max_pwm_/max_rpm_;
        if (tmp_u>max_pwm_){ 
            return max_pwm_; 
        }
        else if (tmp_u<-max_pwm_) {
            return -max_pwm_;
        }
        else { 
            return tmp_u;
        }

        
    }