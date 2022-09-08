#include <P_controller.h>
double P_controller::update(double ref, double actual){
  return k_p*(ref-actual);
}
