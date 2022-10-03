#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <digital_out.h>
#include <timer_usec.h>
#include <timer0_msec.h>
#include <encoder_simple.h>

#include <state.h>
#include <context.h>
#include <parsed.h>
#include <state_stopped.h>
#include <state_pre_op.h>
#include <state_operation.h>
#include <state_initialization.h>

#include <hackySerial.h>
#include <controller.h> 
#include <encoder_interrupt.h>
#include <motor_controller.h>
#include <speedometer.h>



#include<random_defs.h>


#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1

uint8_t b_i; // index, how many characters have been received unsigned char b_t; // first character

#define buffer_size 10
unsigned char b[buffer_size]; // buffer for incoming stuff
unsigned char toolongmsg[] = {'t', 'o', 'o', ' ', 'l', 'o', 'n', 'g'};
Context *context;

void reset_buffer()
{
    for (int i = 0; i< buffer_size; i++)
    {
        b[i] = 0;
    }
    b_i = 0;
}

int main()
{
  USART_Init(0); // 1megabaud
  //USART_Init(MYUBRR); // 9600
  context = new Context(new state_intialization);
  context->timer_u.init();
  context->timer_msec.init(DELTA_T_MS);
  context->encoder_int.init();
  
  while (true)
  {

    if (USART_receive_ready())
    {
      unsigned char inn = USART_Receive();
      // print_i_ln(inn);
      USART_Transmit(inn);
      if (inn == 8)
      {
        // backspace
        if (b_i > 0)
          b_i--;
        continue;
      }
      if (b_i >= buffer_size && inn != '\n')
      {
        println(toolongmsg, sizeof(toolongmsg));
        reset_buffer();
        continue;
      }
      else
      {
        if (inn == '\n')
        {
          if ('A' <= b[0] && b[0] <= 'Z')
          {
            // send command to current state
            if (b[0] == 'I')
              context->transition_to(new state_intialization);
            else if (b[0] == 'P')
              context->transition_to(new state_pre_op);
            else if (b[0] == 'O')
              context->transition_to(new state_operation);
            else if (b[0] == 'S')
              context->transition_to(new state_stopped);
            else
            {
              context->cmd(b[0]);
            }
          }
          else if ('a' <= b[0] && b[0] <= 'z')
          {
            // send parsed to current state
            context->set(parse(b, buffer_size));
          }
          reset_buffer();
        }
        else
        {
          b[b_i++] = inn;
        }
      }
    }
    context->do_work();

  }
}

ISR(INT0_vect)
{

  context->encoder_int.pin1();
  context->delta_counts=context->timer_u.get();
  context->timer_u.reset();

}

ISR(INT1_vect)
{
  context->encoder_int.pin2();
}

ISR(TIMER0_COMPA_vect)
{
  context->flag = true;
  TCNT0 = 0;
}

/*
The idea is to have timer 1 count and everytime we get INT0 or INT1 we check the value of timer 1 and reset timer 1,
after we get value of timer 1 we can get how much time passed between pulses and with 28 pulses per revolution and time t, rpm should be:
rpm = 1/(28pulse/revolution * t/pulse)*60 == revolution/(28*t)*60 == rev/t * 60/28
*/
