#ifndef INIT_ST_DEFINED
#define INIT_ST_DEFINED

#include <state.h>

class state_intialization : public State
{
public:
 ~state_intialization() override;
  void on_do() override;
  void on_entry() override;
  void on_exit() override;
  void set(Parsed p) override;
  void cmd(unsigned char C) override;

};

#endif
