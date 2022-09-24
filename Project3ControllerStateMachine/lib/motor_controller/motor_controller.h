#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_
#include "PWM2.h"
#include "digital_out.h"

PWM2 pwm;
class Motor_controller : PWM2 {
    /* the motor controller should take in duty from the P(I) controller 
    duty will be negative in case of reverse
    it should handle setting pins to reverse/forward/brake motor, (maybe break also)
    */
    public:
    Motor_controller(uint8_t pin1, uint8_t pin2); // only portB allowed 
    void update();
    private:
    Digital_out a1;
    Digital_out a2;
};

    Motor_controller::Motor_controller(uint8_t pin1, uint8_t pin2){
        a1(pin1);
        a2(pin2);
        
    }

#endif
