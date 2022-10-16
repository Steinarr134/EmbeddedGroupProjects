#include <stdint.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
//#include "hackySerial.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


class PI_controller {
    public:
        int16_t update(double set_point, double actual);
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
    public:
        PI_controller(double K_p, double K_i, uint16_t max_rpm, int16_t max_pwm, uint8_t delta_t);
       ~PI_controller() {}
};

void PI_controller::set_kp(double kp)
{
    k_p = kp;
}
void PI_controller::set_ki(double ki)
{
    k_i = ki;
}
void PI_controller::set_kd(double kd)
{
    k_d = kd;
}

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



void export_pwm(uint32_t period, uint32_t initial_duty) { // 0 in, 1 out
    char out[50];
    int len = sprintf(out, "0");
    std::ofstream fOut ("/sys/class/pwm/pwmchip0/export");
    fOut.write(out, len);
    fOut.close();
    usleep(100000);

    
    len = sprintf(out, "%d", period);
    std::ofstream fOut2 ("/sys/class/pwm/pwmchip0/pwm0/period");
    fOut2.write(out, len);
    fOut2.close();
    usleep(100000);

    len = sprintf(out, "%d", initial_duty);
    std::ofstream fOut3 ("/sys/class/pwm/pwmchip0/pwm0/duty_cycle");
    fOut3.write(out, len);
    fOut3.close();
    usleep(100000);

    len = sprintf(out, "%d", 1);
    std::ofstream fOut4 ("/sys/class/pwm/pwmchip0/pwm0/enable");
    fOut4.write(out, len);
    fOut4.close();
    usleep(100000);
}
void set_pwm(int pwm) {
    int period = pwm*(int)1e6/100;

    char out[50];
    int len = sprintf(out, "%d", period);
    std::ofstream fOut ("/sys/class/pwm/pwmchip0/pwm0/duty_cycle");
    fOut.write(out, len);
    fOut.close();
    usleep(100000);
}

std::ifstream fOut ("/dev/mydev");
uint64_t get_pulses() {
    char in[50];
    int i = 0;
    uint64_t amount = 0;
    char* p = in;
    for(i=0;i<32;i++) {
        *p = fOut.get();
        if(*p == '\r') {
            break;
        }
        p++;
    }

    p = in;
    while (*p != '\r') {
        if (*p < '0')
            return 0;
        if (*p > '9')
            return 0;

        amount = (amount * 10) + (*p++ - '0');
    }
    return amount;
}


int main() {
    std::cout<<get_pulses()<<'\n';
    export_pwm(1*1000*1000, 500*1000);
    


    PI_controller pic = PI_controller(12, 100, 15000, 100, 5);
    while(1) {
        break;
    }

    return 0;
}