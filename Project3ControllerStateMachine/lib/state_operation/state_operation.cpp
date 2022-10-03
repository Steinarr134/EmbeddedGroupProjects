#include <random_defs.h>
#include <context.h>
#include <state.h>
#include <hackySerial.h>
#include <state_operation.h>

state_operation:: ~state_operation(){};
void state_operation::on_do() 
  {
    if (context_->flag)
    { // if there's a new measurement available
      context_->dc = context_->delta_counts;
      // convert dc to us, each count is .5µs or .5µs/count => .5µs/count * counts = µs
      double t = TIMER_RESOLUTION * (double)context_->dc;
      if (context_->timer_u.overflow())
      {
        context_->rpm = 0;
      }
      else
      {
        context_->rpm = (int32_t)(double)SECONDS_TO_MINUTE / ((double)PPR * t);
        if (!context_->encoder_int.forward())
        {
          context_->rpm = -context_->rpm;
        }
        if (context_->rpm > MAX_RPM || context_->rpm < MAX_RPM)
        { // at startup rpm can get to some insane number that hacky serial can't even comprehend
          context_->rpm = 0;
        }
      }
      context_->duty = (int16_t)context_->pi_controller.update(context_->set_point, context_->rpm); // RPM of input shaft, not rpm of output shaft!!
      print_3_numbers(context_->set_point, context_->rpm, context_->duty);
      context_->motor_controller.update(context_->duty);
      context_->flag = false;
      context_->count++;
    }
  }
  void state_operation::on_entry() 
  {
    context_->motor_controller.unbrake();
    print_one('O');
    print_one('p');
    println();
  }
  void state_operation::on_exit()  {}
  void state_operation::set(Parsed p)  {}
  void state_operation::cmd(unsigned char C)  {}
