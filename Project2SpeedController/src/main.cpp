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
#define INV_DELTA_T (int32_t)1.0/((float)DELTA_T/1000.0)
#define PPR (int32_t)(7*2*2) // 7 pulses per phase and triggering on falling as well
#define GEAR_REDUCTION (int32_t)101
#define MAX_PWM 255
#define MAX_RPM 15000

uint16_t duty = 0;
int16_t set_point = 2500;
float kp = 4; // gain

//TODO: make KP gain independent of set_point


Encoder_interrupt encoder;
Digital_out led(5);
Timer_msec timer;
PWM2 pwm;
P_controller controller(kp, MAX_RPM, MAX_PWM);
Speedo speedo(DELTA_T);
volatile int32_t counter = 0;
volatile int32_t delta_counts = 0;
volatile bool flag = false;
volatile uint16_t time = 0;
int16_t rpm = 0; // initialize just cause


int main()
{
  USART_Init(0); // 1megabaud
  timer.init(DELTA_T);
  pwm.init();
  pwm.set(0);
  encoder.init();
  
  
  
  // set_interrupt_d1();
  // set_interrupt_d2();
  //float kp = 255.0/80.0*1.2;

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
  unsigned int counter = 0;
  uint8_t oldDuty = 0;
  while (true) {
    
    if(delta_counts <0) {
      delta_counts = -delta_counts;
    }
    delta_counts = delta_counts; 
    rpm = delta_counts * 60 / PPR * INV_DELTA_T;  

    duty = (int16_t)controller.update(set_point, rpm); // RPM of input shaft, not rpm of output shaft!!
    //print_i(delta_counts);
    //println();
    
    duty = (uint8_t)duty&0xFF;
    if(flag) {
      print_3_numbers(set_point, rpm, duty);
      pwm.set(duty);
      flag = false;
    }
    //if(duty != oldDuty) { // I think the timer wasn't happy when compare register was set too often. 
    //}
    if(counter++>10000) {
      counter = 0;
      if(set_point == 5000) {
          set_point = 10000;
      }
      else{
        set_point = 5000;
      }
    }
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
