#include <encoder_simple.h>
#include <avr/delay.h>
#include <digital_out.h>

Encoder_simple::Encoder_simple(){}

void Encoder_simple::init()
{    //PB1, pin 9 on Arduino Nano
    input1.init(1, true);          // use internal pull up
    input2.init(2, true);
}

long Encoder_simple::position()
{
    return counter;
}

void Encoder_simple::monitor()
{
    if (input1.is_hi())
    {
        if (last_state1 == 0)
        {
            if (input2.is_hi())
            {
                counter++;
            }
            else
            {
                counter--;
            }
            last_state1 = 1;
        }
    }
    else
    {
        last_state1 = 0;
    }

    if (input2.is_hi())
    {
        if (last_state2 == 0)
        {

            if (input1.is_hi())
            {
                counter--;
            }
            else
            {
                counter++;
            }

            last_state2 = 1;
        }
    }
    else
    {
        last_state2 = 0;
    }
}