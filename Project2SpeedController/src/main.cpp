#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <digital_out.h>
#include <timer_msec.h>
#include <encoder_simple.h>
#include <hackySerial.h>
#include <P_controller.h>
#include "PWM2.h"
#include <encoder_interrupt.h>

#define DELTA_T (int32_t)10 
#define INV_DELTA_T (int32_t)100 // 1/0.01 = 100
#define PPR (int32_t)(7*2*2) // 7 pulses per phase and triggering on falling as well
#define GEAR_REDUCTION (int32_t)101


Encoder_interrupt encoder;
Digital_out led(5);
Timer_msec timer;
PWM2 pwm;
volatile int32_t counter = 0;
volatile int32_t delta_counts = 0;
volatile bool flag = false;
volatile uint16_t time = 0;
int16_t rpm = 0; // initialize just cause

ISR(TIMER1_COMPA_vect)
{
  delta_counts = encoder.position();
  encoder.reset();
  flag = true;
}

int main()
{
  USART_Init(MYUBRR);
  timer.init(DELTA_T);
  pwm.init();
  pwm.set(0);
  
  
  
  // set_interrupt_d1();
  // set_interrupt_d2();
  //float kp = 255.0/80.0*1.2;
  uint16_t duty = 0;
  int16_t set_point = 10;
  float kp = 255.0*1.2/float(set_point);
  float out;
  _delay_ms(500);

  flag = false;
  while (!flag){}
  pwm.set(255);
  // for (int i = 0; i < 100; i++)
  int i = 0;
  while (true)
  {
    if (flag){
      i++;
      flag = false;
      print_i(delta_counts);
      if (flag){
        print_i(1);
      }
    }
    if (i > 100)
      break;
  }

  while (true) {
    _delay_ms(100); // if duty is set more often it gets stuck at 50% duty
    delta_counts = delta_counts/10; //  delta_counts is 10x the value for some reason
    rpm = delta_counts * 60 / PPR * INV_DELTA_T / GEAR_REDUCTION;  // accurate

    duty = (int16_t)(kp * (float)(set_point - rpm)); // RPM of output shaft, not rpm of input shaft!!
    if(duty>255)duty=255;
    if(duty<0) duty=0;
    pwm.set((uint8_t)duty&0xFF);
  }

  
}

ISR(INT0_vect)
{
  encoder.pin1();
}

ISR(INT1_vect)
{
  encoder.pin2();
}
