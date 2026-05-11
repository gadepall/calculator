
float f(float x) {
    return 1.0f / x;
}

volatile float x = 1.0f;
volatile float y = 0.0f; 
volatile float h = 0.06f;
volatile float result;

#if 0
float backward_euler(float x, float y, float h) {
    return y + h * f(x + h);
}

float rk4(float x, float y, float h) {
    float k1 = f(x);
    float k2 = f(x + h / 2.0f);
    float k3 = f(x + h / 2.0f);
    float k4 = f(x + h);
    
    return y + (h / 6.0f) * (k1 + 2.0f * k2 + 2.0f * k3 + k4);
}

float trapezoidal(float x, float y, float h) {
    return y + (h / 2.0f) * (f(x) + f(x + h));
}
#endif

float forward_euler(float x, float y, float h) {
    return y + h * f(x);
}

int main(void) {
    result = forward_euler(x,y,h);
    while(1) {
        asm volatile("nop"); 
    }
    return 0;
}
