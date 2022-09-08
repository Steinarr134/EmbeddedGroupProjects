#include <P_controller.h>
P_controller::P_controller(double K_p) {
  k_p=K_p;
}

double P_controller::update(double ref, double actual){
  return k_p*(ref-actual);
}
