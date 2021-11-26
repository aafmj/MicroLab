#ifndef F_CPU

#define F_CPU 16000000UL // clock speed is 16MHz

#endif

#define LCD_DATA PORTB // port B is selected as LCD data port
#define ctrl PORTA	   //  port D is selected as LCD command port
#define en PD7		   // enable signal is connected to port D pin 7
#define rw PD6		   // read/write signal is connected to port D pin 6
#define rs PD5		   // register select signal is connected to port D pin 5

#define line2 0xC0
#define line3 0x94
#define line4 0xD4

//**************sending command on LCD***************//

void LCD_cmd(unsigned char cmd)

{

	LCD_DATA = cmd; // data lines are set to send command*

	ctrl &= ~(1 << rs); // RS sets 0

	ctrl &= ~(1 << rw); // RW sets 0

	ctrl |= (1 << en); // make enable from high to low

	_delay_ms(2);

	ctrl &= ~(1 << en);

	return;
}

//*****************write data on LCD*****************//

void LCD_write(unsigned char data)

{

	LCD_DATA = data; // data lines are set to send command

	ctrl |= (1 << rs); // RS sets 1

	ctrl &= ~(1 << rw); // RW sets 0

	ctrl |= (1 << en); // make enable from high to low

	_delay_ms(2);

	ctrl &= ~(1 << en);

	return;
}

void init_LCD(void)

{

	LCD_cmd(0x38); // initialization in 8bit mode of 16X2 LCD

	_delay_ms(1);

	LCD_cmd(0x01); // make clear LCD

	_delay_ms(1);

	LCD_cmd(0x02); // return home

	_delay_ms(1);

	LCD_cmd(0x06); // make increment in cursor

	_delay_ms(1);

	LCD_cmd(0x80); // �8� go to first line and �0� is for 0th position

	_delay_ms(1);

	_delay_ms(10);

	LCD_cmd(0x0E); // display on, cursor blinking

	_delay_ms(10);

	return;
}
//*****************write STRING on LCD*****************//

void LCD_Write_String(char *a)
{
	int i;
	for (i = 0; a[i] != '\0'; i++)
	{
		char big = a[i];
		LCD_write(big);
	}
}

void LCD_Clear()
{
	LCD_cmd(0x01);
}

void LCD_Timing_Write(char *a)
{
	LCD_Clear();
	LCD_Write_String(a);
	_delay_ms(2000);
}

void LCD_Write_Line(int line, char *a)
{
	LCD_cmd(line);
	LCD_Write_String(a);
}