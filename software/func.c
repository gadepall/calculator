#include <stdio.h>
#include <stdlib.h>

#define PI 3.1415926535
#define H  0.01
#define ORD 2

#define DEBUG 0

typedef void (*sys_f)(double t, double *y, double *dy, void *ctx);

double m_abs(double x) { return x < 0 ? -x : x; }

void solve(double t, double t_end, double *y, int dim, sys_f f, void *ctx) {
    double dy[ORD], k1[ORD], k2[ORD], k3[ORD], k4[ORD], yt[ORD];
    double dt = t_end - t;
    int n = (int)(m_abs(dt) / H);
    double h = (dt < 0) ? -H : H;

    for (int step = 0; step <= n; step++) {
        // Remainder handl
        if (step == n) {
            h = t_end - t;
            if (m_abs(h) < 1e-9) break;
        }

        f(t, y, k1, ctx);
        for(int i=0; i<dim; i++) yt[i] = y[i] + k1[i] * h * 0.5;
        
        f(t + h*0.5, yt, k2, ctx);
        for(int i=0; i<dim; i++) yt[i] = y[i] + k2[i] * h * 0.5;
        
        f(t + h*0.5, yt, k3, ctx);
        for(int i=0; i<dim; i++) yt[i] = y[i] + k3[i] * h;
        
        f(t + h, yt, k4, ctx);
        
        for(int i=0; i<dim; i++) 
            y[i] += (h/6.0) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
        
        t += h;
    }
}

void d_sin(double t, double *y, double *dy, void *ctx) {
    dy[0] = y[1];  // y' = z
    dy[1] = -y[0]; // z' = -y
}

void d_ln(double t, double *y, double *dy, void *ctx) {
    dy[0] = 1.0 / t;
}

void d_pow(double t, double *y, double *dy, void *ctx) {
    double w = *(double*)ctx;
    dy[0] = (w / t) * y[0];
}

// my funcs
float Q_sqrt(float n) {
    long i; float x2, y; const float th = 1.5F;
    x2 = n * 0.5F; y = n;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 );
    y = * ( float * ) &i;
    return y * ( th - ( x2 * y * y ) );
}
double m_sqrt(double n) { return 1.0 / Q_sqrt((float)n); }

double m_sin(double x) {
    double y[2] = {0, 1}; // y=0, z=1
    solve(0, x, y, 2, d_sin, NULL);
    return y[0];
}
double m_cos(double x) { return m_sin(PI/2 - x); }
double m_tan(double x) { return m_sin(x) / m_cos(x); }

double m_ln(double x) {
    if (x <= 0) return -1.0/0.0;
    double y[1] = {0};
    solve(1, x, y, 1, d_ln, NULL);
    return y[0];
}

double m_pow(double x, double w) {
    double y[1] = {1};
    solve(1, x, y, 1, d_pow, &w);
    return y[0];
}

double m_fact(double n) {
    return (n <= 1) ? 1 : n * m_fact(n - 1);
}

#ifdef DEBUG
int main(int argc, char *argv[]) {
    int sel = atoi(argv[1]);
    double v = atof(argv[2]);
    double p = 0;

    if (sel == 6) {
        if (argc < 4) { printf("arg num power\n"); return 1; }
        p = atof(argv[3]);
    }

    double out = 0;
    switch (sel) {
        case 1: out = m_sqrt(v); break;
        case 2: out = m_sin(v); break;
        case 3: out = m_cos(v); break;
        case 4: out = m_tan(v); break;
        case 5: out = m_ln(v); break;
        case 6: out = m_pow(v, p); break;
        case 7: out = m_fact(v); break;
        default: printf("Err: 1-7\n"); return 1;
    }
    printf("%lf\n", out);
    return 0;
}
#endif
