#include <state.h>

class state_operation : public State
{
public:

  void on_do();

  void on_entry() override;

  void on_exit() override;

};
