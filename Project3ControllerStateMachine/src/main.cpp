#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "digital_out.h"
#include "timer_usec.h"
#include "timer0_msec.h"
#include "encoder_simple.h"
#include "hackySerial.h"

#include "controller.h" // todo: move PI controller to separate file 
#include "encoder_interrupt.h"
#include "motor_controller.h"
#include "speedometer.h"

#include "state.h"
#include "state_stopped.h"
#include "state_pre_op.h"
#include "state_operation.h"
#include "state_initialization.h"


#define DELTA_T (uint8_t)5
#define INV_DELTA_T (int32_t)(1.0 / ((float)DELTA_T / 1000.0))
#define PPR (int32_t)(7 * 2 ) // 7 pulses per phase and triggering on falling as well, but only usin single phase
#define SECONDS_TO_MINUTE 60
#define GEAR_REDUCTION (int32_t)101
#define MAX_PWM 255
#define MAX_RPM 15000
#define TIMER_RESOLUTION (double)500e-9

int16_t duty = 0;
int16_t set_point = 5000;
double kp = 15; // gain
double ki = 10;//0.01;

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



int main()
{
  USART_Init(0); // 1megabaud
  // USART_Init(MYUBRR); // 9600
  timer_u.init();
  timer_msec.init(DELTA_T);
  encoder.init();
  DDRC |= (1<<0);
  while (true)
  {
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
      PORTC ^= (1<<PORTC0); // A0
    }
    set_point = 0;
    continue;
    if (count > 500)
    { // to vary the set point
      count = 0;
      if (set_point == -5000)
      {
        set_point = 5000;
      }
      else
      {
        set_point = -5000;
      }
    }
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
  //delta_counts = timer_u.get();
  //timer_u.reset();
}

ISR(TIMER0_COMPA_vect)
{
  //delta_counts = encoder.position();
  //encoder.reset();
  flag = true;
  TCNT0 = 0;
  //PORTC ^= (1<<PORTC0); // A0
} 

/*
The idea is to have timer 1 count and everytime we get INT0 or INT1 we check the value of timer 1 and reset timer 1, 
after we get value of timer 1 we can get how much time passed between pulses and with 28 pulses per revolution and time t, rpm should be:
rpm = 1/(28pulse/revolution * t/pulse)*60 == revolution/(28*t)*60 == rev/t * 60/28 
*/