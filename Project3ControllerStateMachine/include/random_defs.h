
#ifndef RAND_DEFS_DEFINED
#define RAND_DEFS_DEFINED

#include <stdint.h>

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1



#define MAX_RPM 15000
#define MAX_PWM 255

#define DELTA_T_MS 5
#define INV_DELTA_T (int32_t)(1.0 / ((float)DELTA_T / 1000.0))
#define PPR 7 * 2 // 7 pulses per phase and triggering on falling as well, but only usin single phase
#define SECONDS_TO_MINUTE 60
#define GEAR_REDUCTION 101
#define TIMER_RESOLUTION 500e-9


#endif
