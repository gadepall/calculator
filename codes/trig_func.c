#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265358979323846
#define H 0.01

double sin(double x_target) {
  double x, y, z;
  double k1, k2, k3, k4, l1, l2, l3, l4;
  double angle = x_target;
  int k = (int)(angle / (2*PI));
    angle -= k * (2*PI);
    if (angle < 0) angle += (2*PI);
  x_target = angle;

  x = 0.0;    
  y = 0.0;    // y(0) = 0
  z = 1.0;    // y'(0) = 1

  while(x < x_target) {
    if (x + H > x_target) {
      double last_h = x_target - x;

      k1 = last_h * z;
      l1 = -last_h * y;

      k2 = last_h * z + l1/2;
      l2 = - (last_h * (y + k1/2));

      k3 = last_h * z + l2/2;
      l3 = - (last_h * (y + k2/2));

      k4 = last_h * z + l3;
      l4 = -(last_h * (y + k3));

      y = y + (k1 + 2*k2 + 2*k3 + k4)/6;
      z = z + (l1 + 2*l2 + 2*l3 + l4)/6;

      x = x_target;
    } else {
      k1 = H *  z;
      l1 = -H * y;

      k2 = H * (z + l1/2);
      l2 = -(H * (y + k1/2));

      k3 = H * (z + l2/2);
      l3 = - H * (y + k2/2);

      k4 = H * (z + l3);
      l4 =  -H * (y + k3);

      y = y + (k1 + 2*k2 + 2*k3 + k4)/6;
      z = z + (l1 + 2*l2 + 2*l3 + l4)/6;

      x = x + H;
    }
  }

  return y;
}

double cos(double x_target) {

  return sin(PI/2 - x_target);
}

double tan(double x){
	return sin(x) / cos(x);
}

