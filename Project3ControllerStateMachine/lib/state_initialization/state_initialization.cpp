#include <state.h>
#include <context.h>
#include <hackySerial.h>

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
