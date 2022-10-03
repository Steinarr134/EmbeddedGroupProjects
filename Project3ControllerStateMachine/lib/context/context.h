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

// #include <random_defs.h>

#include <digital_out.h>
#include <timer_usec.h>
#include <timer0_msec.h>
#include <encoder_simple.h>

// #include <hackySerial.h>
#include <PI_controller.h>
#include <motor_controller.h>
#include <encoder_interrupt.h>
// #include <speedometer.h>


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
double kp = 10;  // gain
double ki = 5.4; // 0.01;



Encoder_interrupt encoder_int;
Digital_out led;
Motor_controller motor_controller;
PI_controller pi_controller;


  Context(State *state);

  ~Context();

  /**
   * The Context allows changing the State object at runtime.
   */

  void transition_to(State *state);

  /**
   * The Context delegates part of its behavior to the current State object.
   */

  void do_work();
  void set(Parsed p);
  void cmd(unsigned char C);
};

#endif
