
#include <state.h>
#include <context.h>
#include <motor_controller.h>
#include <hackySerial.h>
#include <parsed.h>
#include <state_stopped.h>
  state_stopped::~state_stopped(){}
  void state_stopped::on_do()  {}
  void state_stopped::on_entry() 
  {
    context_->motor_controller.brake();
    print_one('S');
    print_one('t');
    print_one('o');
    print_one('p');
    println();
  }
  void state_stopped::on_exit()  {
    context_->motor_controller.brake();
  }
  void state_stopped::set(Parsed p)  {}
  void state_stopped::cmd(unsigned char C)  {}
