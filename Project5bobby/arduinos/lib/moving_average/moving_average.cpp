#include "moving_average.h"
#include <avr/io.h>

void Moving25::reset()
{
    for (int i = 0; i < MOVINGAVGPERIOD; i++)
    {
        buffer[i] = 0;
    }
}

void Moving25::set(long val)
{
    buffer[b_i] = val;
    b_i = (b_i + 1) % MOVINGAVGPERIOD;
}

double Moving25::get()
{
    int64_t sum = 0;
    for (int i = 0; i < MOVINGAVGPERIOD; i++)
    {
        sum += buffer[i];
    }
    return sum / (double)MOVINGAVGPERIOD;
}
