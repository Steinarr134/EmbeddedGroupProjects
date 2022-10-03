#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_
#include <PWM2.h>


class Motor_controller : PWM2 {
    /* the motor controller should take in duty from the P(I) controller 
    duty will be negative in case of reverse
    it should handle setting pins to reverse/forward/brake motor, (maybe break also)
    */
    public:
    Motor_controller(uint8_t pin1, uint8_t pin2); // only portB allowed 
    void update(int16_t pwm);
    void brake();
    void unbrake();
    private:
    uint8_t a1_mask;
    uint8_t a2_mask;
    uint8_t brake_;
};

#endif
