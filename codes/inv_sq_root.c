#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double fast_inv_sqrt(double x){
    x = (float) x;
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = x * 0.5F;
	y  = x;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );

	return (double) y;
}
