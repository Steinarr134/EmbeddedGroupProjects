#include <stdint.h>
//#include "hackySerial.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

class controller {
    public:
        virtual int16_t update(double set_point, double actual);
        void set_kp(double kp);
        void set_ki(double ki);
        void set_kd(double kd);

        double k_p;
        double k_i;
        double k_d;
        int16_t max_rpm_;
        int16_t max_pwm_;
    protected:
    double integral_;
    double delta_t_;
    double u;
    double tmp_u;
};

void controller::set_kp(double kp)
{
    k_p = kp;
}
void controller::set_ki(double ki)
{
    k_i = ki;
}
void controller::set_kd(double kd)
{
    k_d = kd;
}

class PI_controller : public controller {
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
        tmp_u = 0;

    }
    int16_t PI_controller::update(double set_point, double actual) {
        double e = set_point - actual;
        
        //windup guard needs to account for scaling 
        // if(tmp_u < max_pwm_ && tmp_u > -max_pwm_) { // only integrate if pwm is less than 100p 
        //     integral_ += e * delta_t_;
        // }
        integral_ = MAX(-max_pwm_, MIN(max_pwm_, integral_ + e * delta_t_));
        /*print_i((int32_t) u);
        print_one('\t');
        print_i((int32_t) integral_);
        print_one('\t');*/
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