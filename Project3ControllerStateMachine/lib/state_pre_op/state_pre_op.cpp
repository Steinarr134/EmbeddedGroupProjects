#include <state.h>
#include <context.h>
#include <parsed.h>
class state_pre_op : public State
{
public:

  void on_do(){};

  void on_entry() {};

  void on_exit() {};
  void set(Parsed p)  {}
  void cmd(unsigned char C)  {}

};
