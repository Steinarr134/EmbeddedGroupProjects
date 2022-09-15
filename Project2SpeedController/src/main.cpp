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
#include "speedometer.h"


#define DELTA_T (int32_t)10 
#define INV_DELTA_T (int32_t)100 // 1/0.01 = 100
#define PPR (int32_t)(7*2*2) // 7 pulses per phase and triggering on falling as well
#define GEAR_REDUCTION (int32_t)101


Encoder_interrupt encoder;
Digital_out led(5);
Timer_msec timer;
PWM2 pwm;
Speedo speedo(DELTA_T);
volatile int32_t counter = 0;
volatile int32_t delta_counts = 0;
volatile bool flag = false;
volatile uint16_t time = 0;
int16_t rpm = 0; // initialize just cause


int main()
{
  USART_Init(MYUBRR);
  timer.init(DELTA_T);
  pwm.init();
  pwm.set(0);
  encoder.init();
  
  
  
  // set_interrupt_d1();
  // set_interrupt_d2();
  //float kp = 255.0/80.0*1.2;
  uint16_t duty = 0;
  int16_t set_point = 5000;
  float kp = 255.0*.8/float(set_point);
  /*
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
      print_i(speedo.speed());
      if (flag){
        print_i(1);
      }
    }
    if (i > 100)
      break;
  }*/
  int counter = 0;
  uint8_t oldDuty = 0;
  while (true) {
    //_delay_ms(100); // if duty is set more often it gets stuck at 50% duty
    if(delta_counts <0) {
      delta_counts = -delta_counts;
    }
    delta_counts = delta_counts; 
    rpm = delta_counts * 60 / PPR * INV_DELTA_T;  // accurate

    duty = (int16_t)(kp * (float)(set_point - rpm)); // RPM of output shaft, not rpm of input shaft!!
    print_i(delta_counts);
    println();
    
    if(duty>255)duty=255;
    if(duty<0) duty=0;
    duty = (uint8_t)duty&0xFF;
    if(duty != oldDuty) {
      pwm.set(duty);
    }
    /*
    if(counter++ > 5000) {
      counter = 0;
      if(set_point == 5000) {
          set_point = 10000;
      }
      else{
        set_point = 5000;
      }
    }*/
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

ISR(TIMER1_COMPA_vect)
{
  delta_counts = encoder.position();
  encoder.reset();
  flag = true;
}