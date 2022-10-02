#ifndef STATE_DEFINED
#define STATE_DEFINED
#include <parsed.h>


class Context; // Forward declaration to allow the definition of a pointer to Context without compile error

class State
{
  /**
   * @var Context
   */

protected:
  Context *context_;

public:
  virtual ~State()
  {
  }

  void set_context(Context *context)
  {
    this->context_ = context;
  }

  virtual void on_do() = 0;

  virtual void on_entry() = 0;

  virtual void on_exit() = 0;

  virtual void set(Parsed set) = 0;

  virtual void cmd(unsigned char C) = 0;

  // ...
};

/**
 * The Context defines the interface of interest to clients. It also maintains a
 * reference to an instance of a State subclass, which represents the current
 * state of the Context.
 */

#endif
