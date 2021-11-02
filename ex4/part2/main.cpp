#include <avr/io.h>
#include <util/atomic.h>
#include <avr/delay.h>
#include <stdio.h>
#include "lcd.h"

unsigned int overflows_count = 0;

unsigned int second = 0;
unsigned int minute = 0;
unsigned int hour = 0;

char time[9] = "";

void add_second()
{
    second++;

    if (second == 60)
    {
        second = 0;
        minute++;
    }
    if (minute == 60)
    {
        minute = 0;
        hour++;
    }
}

ISR(TIMER2_OVF_vect)
{
    overflows_count++;

    if (overflows_count % 1953 == 0)
    {
        add_second();

        LCD_cmd(0x01); //clear lcd
        sprintf(time, "%d:%d:%d", hour, minute, second);
        LCD_Write_String(time);
    }
}
int main()
{
    DDRB = 0xFF;   // set LCD data port as output
    DDRD = 0xE0;   // set LCD signals (RS, RW, E) as out put
    init_LCD();    // initialize LCD
    LCD_cmd(0x0C); // display on, cursor off

    TIMSK = (1 << TOIE2); // enable timer2 overflow
    TCCR2 = (1 << CS20);  // set timer2 prescaler to clock (no prescaling)
    sei();                // enable all intrupts

    while (1)
    {
    }
    return 0;
}