#include <avr/io.h>
#include <avr/delay.h>
#include <stdio.h>
#include "lcd.h"

uint16_t adc_result0;

char temp_celcius_str[20] = "";
char temp_fahrenheit_str[20] = "";

uint16_t adc_read(uint8_t ch)
{
    ch &= 0b00000111;            
    ADMUX = (ADMUX & 0xF8) | ch; //chooose channel

    ADCSRA |= (1 << ADSC); // start conversation
    
    while (ADCSRA & (1 << ADSC)); // wait for conversion to complete
    
    return (ADC); // read ADCL and ADCW
}

int main()
{

    DDRB = 0xFF;
    DDRD = 0xE0;
    DDRA = 0x0;

    init_LCD();

    LCD_cmd(0x0C);

    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // pre scaler 64 and adc enable

    uint16_t current_adc_result0 = adc_read(0) + 1;

    while (1)
    {
        adc_result0 = adc_read(0);

        if (current_adc_result0 != adc_result0)
        {
            current_adc_result0 = adc_result0;
            int temp = adc_result0 / 2;
            int farTemp = (1.8 * temp) + 32;

            LCD_cmd(0x01);

            sprintf(temp_celcius_str, "celcius: %d", temp);
            sprintf(temp_fahrenheit_str, "fahrenheit: %d", farTemp);
            
            LCD_Write_String(temp_celcius_str);
            LCD_cmd(0xC0);
            LCD_Write_String(temp_fahrenheit_str);
        }
    }
    return 0;
}