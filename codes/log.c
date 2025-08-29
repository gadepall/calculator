#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double ln(double x) {
  if (x <= 0) {
    return 0;
  }
  if (x <= 1){
    return -ln((double) 1/x);
  }

  double x0 = 1.0;
  double y = 0.0;  // ln(1) = 0

  int steps = (int)((x - x0) / H);
  if (x < 1.0) {
    steps = (int)((x0 - x) / H);
    steps = -steps;  // Negative steps for x < 1
  }

  for (int i = 0; i < steps; i++) {
    double k1 = H * (1.0 / x0);
    double k2 = H * (1.0 / (x0 + 0.5 * H));
    double k3 = H * (1.0 / (x0 + 0.5 * H));
    double k4 = H * (1.0 / (x0 + H));

    y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    x0 += H;
  }

  double remaining = x - x0;
  if (remaining > 0) {
    double k1 = remaining * (1.0 / x0);
    double k2 = remaining * (1.0 / (x0 + 0.5 * remaining));
    double k3 = remaining * (1.0 / (x0 + 0.5 * remaining));
    double k4 = remaining * (1.0 / (x0 + remaining));

    y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
  }

  return y;
}
