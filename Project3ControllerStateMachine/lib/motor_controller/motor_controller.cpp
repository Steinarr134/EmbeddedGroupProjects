#include <PWM2.h>
#include<motor_controller.h>
Motor_controller::Motor_controller(uint8_t pin1, uint8_t pin2){ // can't find how to use Digital_out
    a1_mask = (1<<pin1);
    a2_mask = (1<<pin2);
    DDRB |= a1_mask|a2_mask;
    PORTB &= ~a1_mask;
    PORTB &= ~a2_mask;
    this->init();
    
}
void Motor_controller::brake() {
    brake_ = true;
    PORTB |= a1_mask|a2_mask; // if you want to enable the motor shunt brake
    this->set(0); 
}
void Motor_controller::unbrake() {
    PORTB &= ~(a1_mask|a2_mask);
    brake_ = false; 
}
void Motor_controller::update(int16_t pwm) {
    if(brake_) { // don't do anything until brake is released 
        PORTB |= a1_mask|a2_mask; // if you want to enable the motor shunt brake
        this->set(0); 
        return;
    }
    else {
        if(pwm >0) { // clockwise
            PORTB &= ~a2_mask;
            PORTB |= a1_mask;
        }
        else { // counterclockwise
            pwm = -pwm;
            PORTB &= ~a1_mask;
            PORTB |= a2_mask;
        }
    }
    this->set(pwm);
    
}
