#ifndef INIT_ST_DEFINED
#define INIT_ST_DEFINED

#include <state.h>
#include <context.h>
#include <hackySerial.h>
#include <parsed.h>


class state_intialization : public State
{
public:
  void on_do() {}
  void on_entry() 
  {
    print_one('I');
    print_one('n');
    print_one('i');
    print_one('t');
    println();
  }
  void on_exit()  {}
  void set(Parsed p)  {}
  void cmd(unsigned char C)  {}
};

#endif
