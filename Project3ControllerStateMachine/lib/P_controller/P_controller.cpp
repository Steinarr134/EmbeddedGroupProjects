

#include <P_controller.h>
P_controller::P_controller(double K_p) {
  k_p=K_p;
}

uint8_t P_controller::update(double ref, double actual){
  
  double u=k_p*(ref-actual)/max_rpm_;
  u = u*max_pwm_;
  if (u>255) u=255;
  if (u<0) u=0;

  return u;
}
