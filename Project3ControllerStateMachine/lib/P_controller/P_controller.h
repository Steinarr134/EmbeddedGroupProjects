#ifndef P_CONTROLLER_DEFINED
#define P_CONTROLLER_DEFINED
#include<controller.h>
class P_controller : controller{
  public:
    P_controller(double K_p);  
    uint8_t update(double ref, double actual);
};
#endif
