#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "digital_out.h"
#include "timer_usec.h"
#include "timer0_msec.h"
#include "encoder_simple.h"

#include "controller.h" // todo: move PI controller to separate file 
#include "encoder_interrupt.h"
#include "motor_controller.h"
#include "speedometer.h"

#include "state.h"
#include "state_stopped.h"
#include "state_pre_op.h"
#include "state_operation.h"
#include "state_initialization.h"

#include "hackySerial.h"
#include "state.h"
// #include "Arduino.h"
#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1

#define buffer_size 10

#define DELTA_T (uint8_t)5
#define INV_DELTA_T (int32_t)(1.0 / ((float)DELTA_T / 1000.0))
#define PPR (int32_t)(7 * 2 ) // 7 pulses per phase and triggering on falling as well, but only usin single phase
#define SECONDS_TO_MINUTE 60
#define GEAR_REDUCTION (int32_t)101
#define MAX_PWM 255
#define MAX_RPM 15000
#define TIMER_RESOLUTION (double)500e-9

unsigned char b[buffer_size]; // buffer for incoming stuff
unsigned char toolongmsg[] = {'t', 'o', 'o', ' ', 'l', 'o', 'n', 'g'};

// for creating a print statement:
// def p(s):
//     print("print((unsigned char){'" + "', '".join(s) + "'}, " + str(len(s)) + ");")
// def p2(s):
//      for c in s: 
//              print("print_one('" + c + "');")
//      print("println();")


uint8_t b_i; // index, how many characters have been received unsigned char b_t; // first character
int16_t duty = 0;
int16_t set_point = 5000;
double kp = 10; // gain
double ki = 5.4;//0.01;

Encoder_interrupt encoder;
Digital_out led(5);
Motor_controller motor_controller(0, 1);
PI_controller controller(kp, ki, MAX_RPM, MAX_PWM, DELTA_T);
Timer_usec timer_u;
Timer0_msec timer_msec;
volatile int32_t counter = 0;
volatile uint16_t delta_counts = 0;
volatile bool flag = false;
volatile uint16_t time = 0;
int32_t rpm = 0; // initialize just cause
unsigned int count = 0;
uint16_t dc;

Parsed parse()
{
    // b should now be on the form p=1.34\0\0\0\0
    Parsed ret;
    ret.what = b[0];

    double value = 0;
    int tens = 1;
    bool decimal_done = false;
    for (int i = 1; i < buffer_size; i++)
    {
        if (b[i] == '.' || ('0' <= b[i] && b[i] <= '9'))
        {
            if (b[i] == '.')
            {
                decimal_done = true;
                tens = 10;
                continue;
            }
            if (b[i] == 0)
            {
                ret.val = value;
                return ret;
            }
            if (!decimal_done)
            {
                value *= tens;
                value += b[i] - '0';
                tens *= 10;
            }
            else
            {
                value += (double)(b[i] - '0') / tens;
                tens *= 10;
            }
        }
    }
    ret.val = value;
    return ret;
}

void reset_buffer()
{
    for (int i = 0; i< buffer_size; i++)
    {
        b[i] = 0;
    }
    b_i = 0;
}

Context *context;

class Init : public State
    {
    public:
        void on_do() override {}
        void on_entry() override {
            print_one('I');
            print_one('n');
            print_one('i');
            print_one('t');
            println();
        }
        void on_exit() override{}
        void set(Parsed p) override {}
        void cmd(unsigned char C) override {}
    };
class PreOp : public State
    {
    public:
        void on_do() override {}
        void on_entry() override {
            print_one('P');
            print_one('r');
            print_one('e');
            print_one('O');
            print_one('p');
            println();
        }
        void on_exit() override {}
        void set(Parsed p) override {}
        void cmd(unsigned char C) override {}
    };

class Op : public State
    {
    public:
        void on_do() override {
    if (flag)
    { // if there's a new measurement available
      dc = delta_counts; 
      //convert dc to us, each count is .5µs or .5µs/count => .5µs/count * counts = µs
      double t = TIMER_RESOLUTION * (double)dc; 
      if(timer_u.overflow()) {
        rpm = 0;
      }
      else {
        rpm = (int32_t)(double)SECONDS_TO_MINUTE/((double)PPR*t);
        if(!encoder.forward()){
          rpm = -rpm;
        }
        if(rpm > MAX_RPM || rpm < -MAX_RPM) { // at startup rpm can get to some insane number that hacky serial can't even comprehend 
          rpm = 0;
        }

      }
      duty = (int16_t)controller.update(set_point, rpm); // RPM of input shaft, not rpm of output shaft!!
      print_3_numbers(set_point, rpm, duty);
      motor_controller.update(duty);
      flag = false;
      count++;
    }

        }
        void on_entry() override {
            print_one('O');
            print_one('p');
            println();
        }
        void on_exit() override {}
        void set(Parsed p) override {}
        void cmd(unsigned char C) override {}
    };

class Stop : public State
    {
    public:
        void on_do() override {}
        void on_entry() override {
            print_one('S');
            print_one('t');
            print_one('o');
            print_one('p');
            println();
        }
        void on_exit() override {}
        void set(Parsed p) override {}
        void cmd(unsigned char C) override {}
    };





int main()
{
  USART_Init(0); // 1megabaud
  // USART_Init(MYUBRR); // 9600
  timer_u.init();
  timer_msec.init(DELTA_T);
  encoder.init();
  context = new Context(new Init);
  while (true) {

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
            if (b_i >= buffer_size && inn != '\n'){
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
                            context->transition_to(new Init);
                        else if (b[0] == 'P')
                            context->transition_to(new PreOp);
                        else if (b[0] == 'O')
                            context->transition_to(new Op);
                        else if (b[0] == 'S')
                            context->transition_to(new Stop);
                        else
                        {
                            context->cmd(b[0]);
                        }
                        
                    }
                    else if ('a' <= b[0] && b[0] <= 'z')
                    {
                        // send parsed to current state
                        context->set(parse());
                    }
                    reset_buffer();
                }
                else
                {
                    b[b_i++] = inn;
                }
                
            }
        }
 
 
 
    //state operational

  }
}

ISR(INT0_vect)
{
  encoder.pin1();
  delta_counts = timer_u.get();
  timer_u.reset();
}

ISR(INT1_vect)
{
  encoder.pin2();
}

ISR(TIMER0_COMPA_vect)
{
  flag = true;
  TCNT0 = 0;
} 

/*
The idea is to have timer 1 count and everytime we get INT0 or INT1 we check the value of timer 1 and reset timer 1, 
after we get value of timer 1 we can get how much time passed between pulses and with 28 pulses per revolution and time t, rpm should be:
rpm = 1/(28pulse/revolution * t/pulse)*60 == revolution/(28*t)*60 == rev/t * 60/28 
*/