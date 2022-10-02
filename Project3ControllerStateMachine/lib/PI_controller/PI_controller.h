#ifndef PI_CONTROLLER_DEFINED
#define PI_CONTROLLER_DEFINED
#include<controller.h>

class PI_controller : controller {
    public:
        PI_controller(double K_p, double K_i, uint8_t delta_t);
        int16_t update(double set_point, double actual);
};

#endif
