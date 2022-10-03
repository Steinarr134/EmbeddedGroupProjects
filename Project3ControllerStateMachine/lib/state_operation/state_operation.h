#ifndef OP_ST_DEFINED
#define OP_ST_DEFINED

#include <state.h>

class state_operation : public State
{
public:
 ~state_operation() override; 
  void on_do() override;
  void on_entry() override;
  void on_exit() override;
  void set(Parsed p)  override;
  void cmd(unsigned char C)  override;
};
#endif
