
#ifndef STOP_ST_DEFINED
#define STOP_ST_DEFINED
#include <state.h>
#include <parsed.h>
class state_stopped : public State
{
public:

  void on_do() override;
  void on_entry() override;
  void on_exit() override; 
  void set(Parsed p) override; 
  void cmd(unsigned char C) override;
  
};
#endif
