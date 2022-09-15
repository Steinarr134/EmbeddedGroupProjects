#include <P_controller.h>
P_controller::P_controller(double K_p) {
  k_p=K_p;
}

double P_controller::update(double ref, double actual){
  
  int u=k_p*(ref-actual);
  if (u>255) u=255;
  if (u<0) u=0;
  return u;
}
