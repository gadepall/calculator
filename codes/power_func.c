#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define H 0.01
#define EULER_CONST 2.718281828459045

double pow(double x, double w) {
  if (x == 0) return 0;  
  if (w == 0) return 1;  

  double x0 = 1.0;
  double y = 1.0;  // y(1) = 1

  int steps = (int)((x - x0) / H);
  if (x < 1.0) {
    steps = (int)((x0 - x) / H);
    steps = -steps;  // Negative steps for x < 1
  }

  for (int i = 0; i < steps; i++) {
    double k1 = H * (w * y / x0);
    double k2 = H * (w * (y + 0.5 * k1) / (x0 + 0.5 * H));
    double k3 = H * (w * (y + 0.5 * k2) / (x0 + 0.5 * H));
    double k4 = H * (w * (y + k3) / (x0 + H));

    y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    x0 += H;
  }

  double remaining = x - x0;
  if (remaining > 0) {
    double k1 = remaining * (w * y / x0);
    double k2 = remaining * (w * (y + 0.5 * k1) / (x0 + 0.5 * remaining));
    double k3 = remaining * (w * (y + 0.5 * k2) / (x0 + 0.5 * remaining));
    double k4 = remaining * (w * (y + k3) / (x0 + remaining));

    y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
  }

  return y;
}

double exp(double x){
  return pow(EULER_CONST, x);
}
