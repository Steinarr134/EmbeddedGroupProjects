#include <state.h>

class state_operation : public State
{
public:
  void on_do() override
  {
    if (flag)
    { // if there's a new measurement available
      dc = delta_counts;
      // convert dc to us, each count is .5µs or .5µs/count => .5µs/count * counts = µs
      double t = TIMER_RESOLUTION * (double)dc;
      if (timer_u.overflow())
      {
        rpm = 0;
      }
      else
      {
        rpm = (int32_t)(double)SECONDS_TO_MINUTE / ((double)PPR * t);
        if (!encoder.forward())
        {
          rpm = -rpm;
        }
        if (rpm > MAX_RPM || rpm < -MAX_RPM)
        { // at startup rpm can get to some insane number that hacky serial can't even comprehend
          rpm = 0;
        }
      }
      duty = (int16_t)controller.update(set_point, rpm); // RPM of input shaft, not rpm of output shaft!!
      print_3_numbers(set_point, rpm, duty);
      motor_controller.update(duty);
      flag = false;
      count++;
    }
  }
  void on_entry() override
  {
    context_.motor_controller.unbrake();
    print_one('O');
    print_one('p');
    println();
  }
  void on_exit() override {}
  void set(Parsed p) override {}
  void cmd(unsigned char C) override {}

};
