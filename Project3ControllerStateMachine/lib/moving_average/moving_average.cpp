#include "moving_average.h"

void Moving25::reset()
{
    for (int i = 0; i < 25; i++)
    {
        buffer[i] = 0;
    }
}

void Moving25::set(int val)
{
    buffer[b_i] = val;
    b_i = (b_i + 1) % 25;
}

double Moving25::get()
{
    long sum = 0;
    for (int i = 0; i < 25; i++)
    {
        sum += buffer[i];
    }
    return sum / 25.;
}
