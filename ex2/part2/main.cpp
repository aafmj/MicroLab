#define F_CPU 1000000UL /* Define CPU Frequency 1MHz */

#include <avr/io.h>
#include <avr/delay.h>

int main()
{
    DDRB = 0xFF;

    while (true)
    {
        PORTB ^= 0xFF;
        _delay_ms(500);
    }
    return 0;
}
