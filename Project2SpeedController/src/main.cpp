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

#define DELTA_T (int32_t)5
#define INV_DELTA_T (int32_t)1.0 / ((float)DELTA_T / 1000.0)
#define PPR (int32_t)(7 * 2 * 2) // 7 pulses per phase and triggering on falling as well
#define GEAR_REDUCTION (int32_t)101
#define MAX_PWM 255
#define MAX_RPM 15000

uint16_t duty = 0;
int16_t set_point = 5000;
float kp = 6; // gain

Encoder_interrupt encoder;
Digital_out led(5);
Timer_msec timer;
PWM2 pwm;
P_controller controller(kp, MAX_RPM, MAX_PWM);
volatile int32_t counter = 0;
volatile int32_t delta_counts = 0;
volatile bool flag = false;
volatile uint16_t time = 0;
int32_t rpm = 0; // initialize just cause
unsigned int counter = 0;
int32_t dc;

int main()
{
  USART_Init(0); // 1megabaud
  // USART_Init(MYUBRR); // 9600
  timer.init(DELTA_T);
  pwm.init();
  pwm.set(0);
  encoder.init();

  while (true)
  {
    if (flag)
    { // if there's a new measurement available
      dc = delta_counts;
      if (dc < 0)
      {
        dc = -dc;
      }
      rpm = dc * (int32_t)60 * INV_DELTA_T / PPR;
      duty = (int16_t)controller.update(set_point, rpm); // RPM of input shaft, not rpm of output shaft!!
      print_3_numbers(set_point, rpm, duty);
      pwm.set(duty);
      flag = false;
      counter++;
    }

    if (counter > 1000)
    { // to vary the set point
      counter = 0;
      if (set_point == 5000)
      {
        set_point = 10000;
      }
      else
      {
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
