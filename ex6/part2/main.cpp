#include <avr/io.h>
#include <avr/delay.h>
#include <util/atomic.h>
#include <stdio.h>
#include "lcd.h"

uint16_t adc_result0;

unsigned int motor_start_temp = 20;
unsigned int hysteresis_bound = 10;
unsigned int motor_step = 0;
unsigned int overflow_count = 0;
char temp_celcius_str[20] = "";
char temp_fahrenheit_str[20] = "";
bool is_Hot = false;

void step_setter()
{
    switch (motor_step)
    {
    case 1:
        PORTA = 0b01000000;
        break;
    case 2:
        PORTA = 0b11010000;
        break;
    case 3:
        PORTA = 0b10110000;
        break;
    case 4:
        PORTA = 0b00100000;
        motor_step = 0;
        break;
    default:
        PORTA = 0b00000000;
        break;
    }
}
void show_start_temp_and_hysteresis_bound(){
    char start_temp[20] = "";
    char hysteresis[20] = "";

    sprintf(start_temp , "temp: %d" , motor_start_temp);
    sprintf(hysteresis , "bound: %d" , hysteresis_bound);

    LCD_cmd(0x01);
    LCD_Write_String(start_temp);
    LCD_cmd(0xC0);
    LCD_Write_String(hysteresis);
    _delay_ms(1000);
}
uint16_t adc_read(uint8_t ch)
{
    ch &= 0b00000111;            
    ADMUX = (ADMUX & 0xF8) | ch; //chooose channel

    ADCSRA |= (1 << ADSC); // start conversation
    
    while (ADCSRA & (1 << ADSC)); // wait for conversion to complete
    
    return (ADC); // read ADCL and ADCW
}
bool is_system_hot(int temp){
    if(temp >= motor_start_temp){
        return true;
    }else if(temp < (motor_start_temp - hysteresis_bound)){
        return false;
    }else{
        if(is_Hot){
            return true;
        }else{
            return false;
        }
    }

}



ISR(INT0_vect){
    if((PINC & 0x01) != 0){
        motor_start_temp++;
    }
    else if((PINC & 0x02) != 0){
        motor_start_temp--;
    }
    else if((PINC & 0x04) != 0){
        hysteresis_bound++;
    }
    else if((PINC & 0x08) != 0){
        hysteresis_bound--;
    }

    show_start_temp_and_hysteresis_bound(); 
}
ISR(TIMER0_OVF_vect)
{
    overflow_count++;
    TCNT0 = 5;

    if (overflow_count == 400)
    {
        if (is_Hot)
        {
            step_setter();
            motor_step++;
        }
        overflow_count = 0;
    }
}
int main()
{
    DDRB = 0xFF;
    DDRA = 0xF0;
    DDRC = 0x00;
    DDRD = 0xE0;

    init_LCD();

    LCD_cmd(0x0C);

    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // pre scaler 64 and adc enable

    GICR |= 1 << INT0;                    // Enable External Interrupts INT0
    MCUCR |= (1 << ISC00) | (1 << ISC01); // set INT0 to trigger the intrupt on rasing edge

    TCCR0 = (1 << CS00);
    TIMSK |= (1 << TOIE0); // enable timer0 intrupt flag

    sei();

    uint16_t current_adc_result0 = -1;

    while (1)
    {
        adc_result0 = adc_read(0);

        if (current_adc_result0 != adc_result0)
        {
            current_adc_result0 = adc_result0;
            int temp = adc_result0 / 2;
            int farTemp = (1.8 * temp) + 32;

            is_Hot = is_system_hot(temp);


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