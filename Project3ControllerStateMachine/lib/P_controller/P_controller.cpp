#include "P_controller.h"
#include <avr/io.h>
P_controller::P_controller(double K_p, uint16_t max_rpm, uint8_t max_pwm) {
  k_p=K_p;
  max_rpm_ = max_rpm;
  max_pwm_ = max_pwm;
}

uint8_t P_controller::update(double ref, double actual){
  
  double u=k_p*(ref-actual)/max_rpm_;
  u = u*max_pwm_;
  if (u>255) u=255;
  if (u<0) u=0;

  return (uint8_t)u;
}
