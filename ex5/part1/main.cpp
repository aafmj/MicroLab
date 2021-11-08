#include <avr/io.h>
#include <util/atomic.h>

unsigned int overflow_cout = 0;
unsigned int step = 0;

void step_setter()
{
    switch (step)
    {
    case 1:
        PORTA = 0b0100;
        break;
    case 2:
        PORTA = 0b1101;
        break;
    case 3:
        PORTA = 0b1011;
        break;
    case 4:
        PORTA = 0b0010;
        step = 0;
        break;
    default:
        PORTA = 0b0000;
        break;
    }
}

ISR(TIMER0_OVF_vect)
{
    overflow_cout++;
    TCNT0 = 5;

    if (overflow_cout == 500)
    {
        if (PINC != 0x04)
        {
            step_setter();
            step++;
        }
        overflow_cout = 0;
    }
}
int main()
{
    DDRC = 0x00;
    DDRA = 0x0F;

    PORTA = 0x00;

    TCCR0 = (1 << CS00);
    TIMSK |= (1 << TOIE0);

    sei();

    while (1)
    {
    }
    return 0;
}