#include <avr/io.h>
#include <util/atomic.h>

unsigned int overflow_cout = 0;
unsigned int step = 0;

bool is_forward = false;
int switch_current_state = 0;
bool is_start = false;

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
    if (!is_start)
    {
        return;
    }
    
    if (overflow_cout % 500 == 0)
    {
        if(is_forward){
            step++;
        }else{
            step--;

            if(step == 0){ // step validation. 
                step = 7;
            }
        }
        step_setter(step);
    }
    overflow_cout++;
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
        if(PINC == 0x04){
            switch_current_state = 0;
        }
        if(PINC != 0x04 && switch_current_state == 0){
            is_forward = !is_forward;
            switch_current_state = 1;

            is_start =true; // start to rotate for the first time
        }
    }
    return 0;
}