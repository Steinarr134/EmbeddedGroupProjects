#ifndef PREOP_ST_DEFINED
#define PREOP_ST_DEFINED

#include <state.h>
#include <context.h>
#include <parsed.h>
#include <state_pre_op.h>

state_pre_op::~state_pre_op(){};
void state_pre_op::on_do(){};

void state_pre_op::on_entry() {};

void state_pre_op::on_exit() {};
void state_pre_op::set(Parsed p)  {}
void state_pre_op::cmd(unsigned char C)  {}

#endif
