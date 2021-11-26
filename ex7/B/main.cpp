#include <avr/io.h>
#include <avr/delay.h>
#include <util/atomic.h>
#include <stdio.h>

unsigned int step = 0;
int overflow_count = 0;
int temp = 0;

uint16_t adc_read(uint8_t ch);
void step_setter();
void rotate_motor(int step_number);


ISR(USART_RXC_vect)
{
    int steps = UDR;
    PORTC = 0x01;
    if(steps == 0){
        overflow_count = 0;
        UDR = adc_read(0) / 2;
        return;
    }
    rotate_motor(steps);
}
ISR(TIMER0_OVF_vect)
{
    overflow_count++;
    TCNT0 = 6;
    if (overflow_count == 4000)
    {
        overflow_count = 0;
        PORTC = 0x02;
    }
}
int main()
{
    DDRB = 0x0F;
    DDRA = 0xF0;
    DDRC = 0x03;

    UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
    UBRRL = 7;

    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // pre scaler 64 and adc enable

    TCCR0 = (1 << CS00);
    TIMSK = (1 << TOIE0);
    TCNT0 = 6;

    PORTB = 0x00;

    sei();

    while (1)
    {
    
    }
    return 0;
}

uint16_t adc_read(uint8_t ch)
{
    ch &= 0b00000111;
    ADMUX = (ADMUX & 0xF8) | ch; //chooose channel

    ADCSRA |= (1 << ADSC); // start conversation

    while (ADCSRA & (1 << ADSC))
        ; // wait for conversion to complete

    return (ADC); // read ADCL and ADCW
}
void step_setter()
{
    switch (step)
    {
    case 1:
        PORTB = 0b0100;
        break;
    case 2:
        PORTB = 0b1101;
        break;
    case 3:
        PORTB = 0b1011;
        break;
    case 4:
        PORTB = 0b0010;
        step = 0;
        break;
    default:
        PORTB = 0b0000;
        break;
    }
}
void rotate_motor(int step_number)
{
    for (int i = 0; i < step_number; i++)
    {
        step++;
        step_setter();
        _delay_ms(500);
    }
}