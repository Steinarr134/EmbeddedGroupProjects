#include <state.h>

class state_pre_op : public State
{
public:
  ~state_pre_op() override;
  void on_do() override;;

  void on_entry() override;

  void on_exit() override;
  void set(Parsed p) override;
  void cmd(unsigned char C)  override;

};
