#include <state.h>

class state_stopped : public State
{
public:

  void on_do();

  void on_entry() override;

  void on_exit() override;

  
};
