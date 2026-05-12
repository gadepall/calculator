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
volatile float c;
volatile float a = 5.0f, b = 3.0f;
void div() {
    c=a+b;
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

    div();

    end_cycles = TCNT1; 

    total_cycles = end_cycles - start_cycles;
    
    printf("Algorithm took: %u cycles\n", total_cycles);
    
    // Sleep
    cli();
    while(1);
}
