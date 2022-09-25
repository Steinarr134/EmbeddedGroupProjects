#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "digital_out.h"
#include "timer_msec.h"
#include "encoder_simple.h"
#include "hackySerial.h"

#include "controller.h" // todo: move PI controller to separate file 
#include "encoder_interrupt.h"
#include "motor_controller.h"

#define DELTA_T (uint8_t)5
#define INV_DELTA_T (int32_t)(1.0 / ((float)DELTA_T / 1000.0))
#define PPR (int32_t)(7 * 2 * 2) // 7 pulses per phase and triggering on falling as well
#define GEAR_REDUCTION (int32_t)101
#define MAX_PWM 255
#define MAX_RPM 15000

int16_t duty = 0;
int16_t set_point = 6000;
double kp = 10; // gain
double ki = 5.4;//0.01;

Encoder_interrupt encoder;
Digital_out led(5);
Timer_msec timer;
Motor_controller motor_controller(0, 1);
PI_controller controller(kp, ki, MAX_RPM, MAX_PWM, DELTA_T);
volatile int32_t counter = 0;
volatile int32_t delta_counts = 0;
volatile bool flag = false;
volatile uint16_t time = 0;
int32_t rpm = 0; // initialize just cause
unsigned int count = 0;
int32_t dc;

int main()
{
  USART_Init(0); // 1megabaud
  // USART_Init(MYUBRR); // 9600
  timer.init(DELTA_T);
  encoder.init();

  while (true)
  {
    if (flag)
    { // if there's a new measurement available
      dc = delta_counts;
      rpm = dc * (int32_t)60 * INV_DELTA_T / PPR;
      duty = (int16_t)controller.update(set_point, rpm); // RPM of input shaft, not rpm of output shaft!!
      print_3_numbers(set_point, rpm, duty);
      motor_controller.update(duty);
      flag = false;
      count++;
    }

    if (count > 500)
    { // to vary the set point
      count = 0;
      if (set_point == 2500)
      {
        set_point = -5000;
      }
      else
      {
        set_point = 2500;
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
