#ifndef CONTEXT_DEFINED
#define CONTEXT_DEFINED
/**
 * The base State class declares methods that all concrete states should
 * implement and also provides a backreference to the Context object, associated
 * with the State. This backreference can be used by States to transition the
 * Context to another State.
 */

struct parsedstruct
{
  unsigned char what;
  double val;
};

typedef struct parsedstruct Parsed;

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

class Context
{
  /**
   * @var State A reference to the current state of the Context.
   */

private:
  State *state_;

public:
  Context(State *state) : state_(nullptr)
  {
    this->transition_to(state);
  }

  ~Context()
  {
    delete state_;
  }

  /**
   * The Context allows changing the State object at runtime.
   */

  void transition_to(State *state)
  {
    if (this->state_ != nullptr)
    {
      this->state_->on_exit();
      delete this->state_;
    }

    this->state_ = state;

    this->state_->set_context(this);

    this->state_->on_entry();
  }

  /**
   * The Context delegates part of its behavior to the current State object.
   */

  void do_work()
  {
    this->state_->on_do();
  }

  void set(Parsed p)
  {
    this->state_->set(p);
  }
  void cmd(unsigned char C)
  {
    this->state_->cmd(C);
  }
};

#endif
