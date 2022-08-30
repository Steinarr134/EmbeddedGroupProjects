#include <avr/delay.h>
#include <digital_in.h>
#include <digital_out.h>

// Part 1
int main()
{
  Digital_out led(5); // PB5 Arduino Nano built-in LED on D13
  led.init();

  Digital_in input(1); // PB1, pin 9 on Arduino Nano
  input.init(true); // use internal pull up
  
  int counter = 0;
  bool last_state = 0;
  while (true){
  // if input changed

    if (input.is_hi() != last_state)
    {
      counter++;
      last_state = !last_state;

      // blink led to signal 
      led.set_hi();
      _delay_us(100);
      led.set_lo();
    }
  }
}