#include <avr/io.h>

class P_controller{
  private:
  double k_p;
  uint16_t max_rpm_;
  uint8_t max_pwm_;
  public:
    P_controller(double K_p, uint16_t MAX_RPM, uint8_t MAX_PWM);  
    uint8_t update(double ref, double actual);
};
