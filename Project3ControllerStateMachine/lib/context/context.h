#ifndef CONTEXT_DEFINED
#define CONTEXT_DEFINED
#include <state.h>
#include <parsed.h>
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



/**
 * The base State class declares methods that all concrete states should
 * implement and also provides a backreference to the Context object, associated
 * with the State. This backreference can be used by States to transition the
 * Context to another State.
 */
 class State;

class Context
{
  /**
   * @var State A reference to the current state of the Context.
   */

private:
  State *state_;

public:

Timer_usec timer_u;
Timer0_msec timer_msec;
volatile int32_t counter = 0;
volatile uint16_t delta_counts = 0;
volatile bool flag = false;
volatile uint16_t time = 0;
int32_t rpm = 0; // initialize just cause
unsigned int count = 0;
uint16_t dc;
int16_t duty = 0;
int16_t set_point = 5000;


Encoder_interrupt encoder_int;
Digital_out led(5);
Motor_controller motor_controller(0, 1);
PI_controller pi_controller;


  Context(State *state) : state_(nullptr)
  {
    // this->motor_controller.in;
    this->led.init();
    this->pi_controller(this.kp, this.ki, DELTA_T_MS);
    this->transition_to(state);
  }

  ~Context()
  {
    delete state_;
  }

  /**
   * The Context allows changing the State object at runtime.
   */

  void transition_to(State *state)
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

  void do_work()
  {
    this->state_->on_do();
  }

  void set(Parsed p)
  {
    this->state_->set(p);
  }
  void cmd(unsigned char C)
  {
    this->state_->cmd(C);
  }
};

#endif
