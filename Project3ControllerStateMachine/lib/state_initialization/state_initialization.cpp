#include <state.h>
#include <context.h>
#include <hackySerial.h>
#include <parsed.h>
#include <state_initialization.h>



  state_intialization::~state_intialization(){}
  void state_intialization::on_do() {}
  void state_intialization::on_entry() 
  {
    print_one('I');
    print_one('n');
    print_one('i');
    print_one('t');
    println();
  }
  void state_intialization::on_exit()  {}
  void state_intialization::set(Parsed p)  {}
  void state_intialization::cmd(unsigned char C)  {}
