#include <PI_controller.h>

PI_controller::PI_controller(double K_p, double K_i, uint8_t delta_t) {
    k_p = K_p;
    k_i = K_i;
    integral_ = 0;
    delta_t_ = (double)delta_t / 1000.0;
    tmp_u = 0;

}
int16_t PI_controller::update(double set_point, double actual) {
    double e = set_point - actual;
    
    //windup guard needs to account for scaling 
    if(tmp_u < max_pwm_ && tmp_u > -max_pwm_) { // only integrate if pwm is less than 100p 
        integral_ += e * delta_t_;
    }
    
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
