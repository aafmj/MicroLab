#include <avr/io.h>
#include <util/atomic.h>

unsigned int timer0_overflow_cout = 0;
unsigned int timer1_overflow_count = 0;

unsigned int step = 0;

bool is_forward = true;

unsigned int speed_num = 4000;
bool is_speed_up = true;

void motor_config()
{
    int change_speed_step = 400;
    if (speed_num < 2000)
    {
        change_speed_step = 50;
    }
    else if (speed_num < 1000)
    {
        change_speed_step = 5;
    }
    // speed config
    if (is_speed_up && speed_num >= 300)
    {
        speed_num = speed_num - change_speed_step;
    }
    else if (is_speed_up && speed_num < 300)
    {
        is_speed_up = false;
    }
    else if (!is_speed_up && speed_num <= 4000)
    {
        speed_num = speed_num + change_speed_step;
    }
    else if (!is_speed_up && speed_num > 4000)
    {
        is_speed_up = true;
        is_forward = !is_forward;
    }

    // step config
    if (is_forward)
    {
        step++;
    }
    else
    {
        step--;

        if (step == 0)
        { // step validation.
            step = 7;
        }
    }
}
void step_setter(int step)
{
    int mode = step % 8;

    switch (mode)
    {
    case 0:
        PORTA = 0b010;
        break;
    case 1:
        PORTA = 0b0110;
        break;
    case 2:
        PORTA = 0b0100;
        break;
    case 3:
        PORTA = 0b0101;
        break;
    case 4:
        PORTA = 0b1101;
        break;
    case 5:
        PORTA = 0b1001;
        break;
    case 6:
        PORTA = 0b1011;
        break;
    case 7:
        PORTA = 0b1010;
        break;
    default:
        PORTA = 0b0000;
        break;
    }
}

ISR(TIMER0_OVF_vect)
{
    if (PINC == 0x04)
    {
        return;
    }
    if (timer0_overflow_cout == speed_num)
    {
        motor_config();
        step_setter(step);
        timer0_overflow_cout = 0;
    }
    timer0_overflow_cout++;
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