#ifndef CONTEXT_DEFINED
#define CONTEXT_DEFINED
/**
 * The base State class declares methods that all concrete states should
 * implement and also provides a backreference to the Context object, associated
 * with the State. This backreference can be used by States to transition the
 * Context to another State.
 */

#include "digital_out.h"
#include "timer_usec.h"
#include "timer0_msec.h"
#include <encoder_simple.h>

#include <hackySerial.h>
#include <controller.h> // todo: move PI controller to separate file
#include <motor_controller.h>
#include <encoder_interrupt.h>
#include <speedometer.h>

#define MAX_PWM 255
#define MAX_RPM 15000
#define TIMER_RESOLUTION (double)500e-9


// for creating a print statement:
// def p(s):
//     print("print((unsigned char){'" + "', '".join(s) + "'}, " + str(len(s)) + ");")
// def p2(s):
//      for c in s:
//              print("print_one('" + c + "');")
//      print("println();")

uint8_t b_i; // index, how many characters have been received unsigned char b_t; // first character
int16_t duty = 0;
int16_t set_point = 5000;
double kp = 10;  // gain
double ki = 5.4; // 0.01;

/**
 * The base State class declares methods that all concrete states should
 * implement and also provides a backreference to the Context object, associated
 * with the State. This backreference can be used by States to transition the
 * Context to another State.
 */

extern class State;

class Context
{
    /**
     * @var State A reference to the current state of the Context.
     */

private:
    State *state_;

public:
    Encoder_simple encoder;

    Context(State *state) : state_(nullptr)
    {
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
