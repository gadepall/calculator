#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

// Simple UART output
int uart_putchar(char c, FILE *stream) {
    if (c == '\n') uart_putchar('\r', stream);
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
    return 0;
}
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

// Algo code
float f(float x) {
    return 1.0f / x;
}

volatile float x = 1.0f;
volatile float y = 0.0f; 
volatile float h = 0.06f;
volatile float result;

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

float forward_euler(float x, float y, float h) {
    return y + h * f(x);
}
int main(void) {
    uint16_t start_cycles, end_cycles, total_cycles;

    UCSR0B = (1 << TXEN0); 
    stdout = &uart_output;

    // Initialize Timer1 (Normal mode, Prescaler = 1)
    TCCR1A = 0;
    TCCR1B = (1 << CS10); 

    printf("Starting Benchmark...\n");

    TCNT1 = 0; 
    start_cycles = TCNT1; 

    result = rk4(x,y,h);

    end_cycles = TCNT1; 

    total_cycles = end_cycles - start_cycles;
    
    printf("Algorithm took: %u cycles\n", total_cycles);
    
    // Sleep
    cli();
    while(1);
}

