#include "math.h"

#ifndef __A2_MATH_H
#define __A2_MATH_H

#define BIG_INT 1073741824

int a2_math_square_fallof(int x, int wave_size, int fall_off) {
    x           = abs(x)        ;
    wave_size   = abs(wave_size);
    fall_off    = abs(fall_off) ;
	
    assert(wave_size != 0 && "FACTOR CANNOT BE 0");
	int f = fabs(wave_size-((x*x)/pow(wave_size,fall_off)));
	
    // prevent explosion on grabage values
    return (f < BIG_INT) ? f : 1;
}

#endif//__A2_MATH_H
