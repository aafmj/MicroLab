char* table[] = {
    "1", "2", "3", 
    "4", "5", "6", 
    "7", "8", "9", 
    "*", "0", "#"};

void keypad_init()
{
    DDRC = 0xF8;
}

char key_released()
{
    PORTC = 0xF8;
    _delay_ms(3);
    if ((PINC & 0x07) == 0x00)
        return 1;
    else
        return 0;
}

char key_pressed()
{
    PORTC = 0xF8;
    _delay_ms(3);
    if ((PINC & 0x07) != 0x00)
    {
        _delay_ms(20);
        if ((PINC & 0x07) != 0x00)
            return 1;
    }
    return 0;
}

char *key_scan()
{
    if (key_pressed() == 1)
    {
        char* key = "";
        for (char i = 3; i < 7; i++)
        {
            PORTC = 1 << i;
            _delay_ms(3);
            if ((PINC & 0x01) > 0)
                key = table[(i - 3) * 3 + 2];
                // PORTA = 0x02;
            if ((PINC & 0x02) > 0)
                key = table[(i - 3) * 3 + 1];
                // PORTA = 0x01;
            if ((PINC & 0x04) > 0)
                key = table[(i - 3) * 3];
                // PORTA = 0x00;
            // if ((PINC & (1 << PIN7)) > 0)
            //     key = table[i * 4 + 3];
        }
        while (!key_released())
            ;
        
        return key;
    }
    else
        return "";
}