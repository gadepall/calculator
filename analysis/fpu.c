#include <avr/sleep.h>

int main(void) {
    volatile int a = 5, b = 3, c;
    c = a / b;
    sleep_mode();
    return 0;
}
