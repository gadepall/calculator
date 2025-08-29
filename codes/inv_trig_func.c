#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265358979323846
#define H 0.01

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

double arctan(double x) {
    double x0 = 0.0, y = 0.0;
    int steps = x >= 0 ? (int) (x / H): (int) (-x / H); 
    double step_dir = (x >= 0) ? 1 : -1;
    
    for (int i = 0; i < steps; i++) {
        double k1 = H / (1 + x0*x0);
        double k2 = H / (1 + (x0 + H/2)*(x0 + H/2));
        double k3 = H / (1 + (x0 + H/2)*(x0 + H/2));
        double k4 = H / (1 + (x0 + H)*(x0 + H));
        
        y += step_dir * (k1 + 2*k2 + 2*k3 + k4) / 6;
        x0 += step_dir * H;
    }
    
    return y;
}
double arcsin(double x) {
    if (x < -1 || x > 1) return 0;

    double x0 = 0.0, y = 0.0;
    int steps = x >= 0 ? (int) (x / H): (int) (-x / H); 
    double step_dir = (x >= 0) ? 1 : -1;

    for (int i = 0; i < steps; i++) {
        double k1 = H * fast_inv_sqrt(1 - x0*x0);
        double k2 = H * fast_inv_sqrt(1 - (x0 + step_dir*H/2)*(x0 + step_dir*H/2));
        double k3 = H * fast_inv_sqrt(1 - (x0 + step_dir*H/2)*(x0 + step_dir*H/2));
        double k4 = H * fast_inv_sqrt(1 - (x0 + step_dir*H)*(x0 + step_dir*H));

        y += step_dir * (k1 + 2*k2 + 2*k3 + k4) / 6;
        x0 += step_dir * H;
    }

    return y;
}

double arccos(double x){
    return ((PI/2) - arcsin(x));
}
