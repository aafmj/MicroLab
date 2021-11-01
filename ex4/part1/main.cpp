#include <avr/io.h>
#include <avr/delay.h>
#include "lcd.h"

int main() {

    DDRB = 0xFF;
    DDRD = 0xE0;

    init_LCD();

    _delay_ms(10);

	LCD_cmd(0x0E);          // display on, cursor blinking

	_delay_ms(10);
	

	LCD_Write_String("  HELLO WORLD  ");
    while (1) {
        
    }
    return 0;
}