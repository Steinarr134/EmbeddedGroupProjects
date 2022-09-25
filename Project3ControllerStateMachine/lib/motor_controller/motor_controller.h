#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_
#include "PWM2.h"

PWM2 pwm;
class Motor_controller : PWM2 {
    /* the motor controller should take in duty from the P(I) controller 
    duty will be negative in case of reverse
    it should handle setting pins to reverse/forward/brake motor, (maybe break also)
    */
    public:
    Motor_controller(uint8_t pin1, uint8_t pin2); // only portB allowed 
    void update(uint16_t pwm);
    private:
    uint8_t a1_mask;
    uint8_t a2_mask;
    uint8_t brake;
};

    Motor_controller::Motor_controller(uint8_t pin1, uint8_t pin2){ // can't find how to use Digital_out
        a1_mask = (1<<pin1);
        a2_mask = (1<<pin2);
        DDRB |= a1_mask|a2_mask;
        PORTB &= ~a1_mask;
        PORTB &= ~a2_mask;
        this->init();
        
    }
    void Motor_controller::update(uint16_t pwm) {
        if(brake) {
            PORTB |= a1_mask|a2_mask;
            this->set(0); // apparently it's enough to have PWM == 0  to brake but we're not very familiar with the PWM generation, maybe it does something weird when pwm == 0;
            return;
        }
        else {
            if(pwm >0) { // clockwise
                PORTB &= a2_mask;
                PORTB |= a1_mask;
            }
            else { // counterclockwise
                PORTB &= a1_mask;
                PORTB |= a2_mask;
            }
        }
        this->set(pwm);
        
    }

#endif
