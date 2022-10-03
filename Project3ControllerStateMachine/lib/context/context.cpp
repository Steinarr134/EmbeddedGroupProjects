
#include <state.h>
#include <parsed.h>
#include <context.h>
/**
 * The base State class declares methods that all concrete states should
 * implement and also provides a backreference to the Context object, associated
 * with the State. This backreference can be used by States to transition the
 * Context to another State.
 */

#include <random_defs.h>

#include <digital_out.h>
#include <timer_usec.h>
#include <timer0_msec.h>
#include <encoder_simple.h>

#include <hackySerial.h>
#include <PI_controller.h>
#include <motor_controller.h>
#include <encoder_interrupt.h>
#include <speedometer.h>




Context::Context(State *state) : state_(nullptr), led(5), motor_controller(0, 1), pi_controller(this->kp, this->ki, DELTA_T_MS)
{
  // this->motor_controller.in;
  this->led.init();
  this->transition_to(state);
}

Context::~Context()
{
  delete state_;
  
}

/**
 * The Context allows changing the State object at runtime.
 */

void Context::transition_to(State *state)
{
  if (this->state_ != nullptr)
  {
    this->state_->on_exit();
    delete this->state_;
  }

  this->state_ = state;

  this->state_->set_context(this);

  this->state_->on_entry();
}

/**
 * The Context delegates part of its behavior to the current State object.
 */

void Context::do_work()
{
  this->state_->on_do();
}

void Context::set(Parsed p)
{
  this->state_->set(p);
}
void Context::cmd(unsigned char C)
{
  this->state_->cmd(C);
}
