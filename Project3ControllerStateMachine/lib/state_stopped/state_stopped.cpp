#ifndef STOPPED_ST_DEFINED
#define STOPPED_ST_DEFINED
#include <state.h>
#include <context.h>
#include <motor_controller.h>
#include <hackySerial.h>
#include <parsed.h>

class state_stopped : public State
{
public:

  void on_do()  {}
  void on_entry() 
  {
    context_->motor_controller.brake();
    print_one('S');
    print_one('t');
    print_one('o');
    print_one('p');
    println();
  }
  void on_exit()  {
    context_->motor_controller.brake();
  }
  void set(Parsed p)  {}
  void cmd(unsigned char C)  {}
};
#endif
