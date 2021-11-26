#include <string.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/delay.h>
#include <util/atomic.h>
#include <stdio.h>
#include "lcd.h"
#include "keypad.h"

// {
// state introduction
// state = 0 ---> login page
// state = 1 ---> menu page
// state = 2 ---> get information page
// }
char lcdInput[10] = "";

const char max_user = 2;
char users_count = 1;
char timer0Amount = 6;
short overflow_count = 0;
char second = 0;
char minute = 0;
char hour = 0;
short year = 2021;
char month = 5;
char day = 2;
char temp = 25;

struct User
{
    char id[5];
    char password[5];
};
User users[max_user] = {{"1", "2"}};

char state = 0;

char is_admin = -1; // -1 means no admin nor user logged in

void login_page();
void menu_page();
void get_info_page();
void date_setting_page();
void remove_user_page();
void add_user_page();
void rotate_motor_page();
void logout();

void print_info();
void add_time();
void add_user(char *id, char *pass);
char *input(int line);

ISR(TIMER0_OVF_vect)
{
    overflow_count++;
    TCNT0 = timer0Amount;
    if (overflow_count == 4000)
    {
        overflow_count = 0;
        add_time();

        if (state == 2)
        {
            UDR = 0;
            print_info();
        }
    }
}
ISR(USART_RXC_vect)
{
    temp = UDR;
}
int main()
{

    DDRD = DDRD | 0xe0;
    DDRB = 0xFF;
    DDRC = 0xF8;
    DDRA = 0xFF;

    UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
    UBRRL = 7;

    TCCR0 = (1 << CS00);
    TIMSK = (1 << TOIE0);
    TCNT0 = timer0Amount;

    keypad_init();
    init_LCD();

    sei();

    LCD_Write_String("Hi.use # to submit");
    _delay_ms(2000);

    while (true)
    {
        switch (state)
        {
        case 0:
            login_page();
            break;

        case 1:
            menu_page();
            break;
        }
    }
    return 0;
}

void add_time()
{
    second++;

    if (second == 60)
    {
        minute++;
        second = 0;
    }
    if (minute == 60)
    {
        hour++;
        minute = 0;
    }
    if (hour == 24)
    {
        hour = 0;
    }
}
char *input(int line)
{
    LCD_cmd(line);
    sprintf(lcdInput, "%s", "\0");

    while (true)
    {
        char *key = key_scan();

        if (strcmp(key, "#") == 0)
        {
            return lcdInput;
        }
        LCD_Write_String(key);
        sprintf(lcdInput, "%s%s", lcdInput, key);
    }
}
void print_info()
{
    char time[20] = "";
    char date[20] = "";
    char temperature[10] = "";

    LCD_Clear();

    sprintf(time, "Time: %d:%d:%d", hour, minute, second);
    sprintf(date, "Date: %d/%d/%d", year, month, day);
    sprintf(temperature, "temp: %dC", temp);

    LCD_Write_String(time);
    LCD_Write_Line(line2, date);
    LCD_Write_Line(line3, temperature);
    LCD_Write_Line(line4, "press * to exit");
}

void rotate_motor_page()
{
    LCD_Clear();
    char steps[5] = "";

    LCD_Write_String("Enter step:");
    char *step_scanned = input(line2);
    sprintf(steps, "%s", step_scanned);

    char step_count = atoi(steps);
    UDR = step_count;

    LCD_Timing_Write("motor rotated ...");
}
void date_setting_page()
{

    LCD_Clear();

    {
        LCD_Write_String("Enter the year:");
        char newYear[10] = "";
        char *year_scanned = input(line2);
        sprintf(newYear, "%s", year_scanned);
        year = atoi(newYear);
    }

    {
        char newMonth[10] = "";
        LCD_Write_Line(line3, "Enter the month:");
        char *month_scanned = input(line4);
        sprintf(newMonth, "%s", month_scanned);
        month = atoi(newMonth);
    }

    LCD_Clear();

    {
        char newDay[10] = "";
        LCD_Write_String("Enter the day:");
        char *day_scanned = input(line2);
        sprintf(newDay, "%s", day_scanned);
        day = atoi(newDay);
    }
    {
        char newHour[10] = "";
        LCD_Write_Line(line3, "Enter the hour:");
        char *hour_scanned = input(line4);
        sprintf(newHour, "%s", hour_scanned);
        hour = atoi(newHour);
    }
    LCD_Clear();
    {
        char newMinute[10] = "";
        LCD_Write_String("Enter the minute:");
        char *minute_scanned = input(line2);
        sprintf(newMinute, "%s", minute_scanned);
        minute = atoi(newMinute);
    }
}
void get_info_page()
{
    while (true)
    {
        char *key = key_scan();
        if (strcmp(key, "*") == 0)
        {
            state = 1;
            break;
        }
    }
}
void login_page()
{
    char id[5] = "";
    char password[5] = "";

    LCD_Clear();
    LCD_Write_String("Enter your id:");
    char *id_scaned = input(line2);
    sprintf(id, "%s", id_scaned);
    LCD_Write_Line(line3, "Enter your password:");
    char *password_scaned = input(line4);
    sprintf(password, "%s", password_scaned);

    for (int i = 0; i < users_count; i++)
    {
        if (strcmp(users[i].id, id) == 0 && strcmp(users[i].password, password) == 0)
        {
            PORTA = 1 << PA1;
            if (i == 0)
            {
                is_admin = 1;
            }
            else
            {
                is_admin = 0;
            }
            state = 1;
            LCD_Timing_Write("successfully logined...");
            PORTA = 0x00;
            return;
        }
    }
    PORTA = 1 << PA0;
    LCD_Timing_Write("Access denied...");
    PORTA = 0x00;
}
void menu_page()
{
    char menu_page_num = 0;

    while (menu_page_num != -1)
    {
        LCD_Clear();

        if (is_admin == 1)
        {
            if (menu_page_num == 0)
            {
                LCD_Write_String("1.see information");
                LCD_Write_Line(line2, "2.rotate motor");
                LCD_Write_Line(line3, "3.date&time setting");
                LCD_Write_Line(line4, "use # for next page");
            }
            else
            {
                LCD_Write_String("1.Add or Edit user");
                LCD_Write_Line(line2, "2.remove user");
                LCD_Write_Line(line3, "3.logout");
                LCD_Write_Line(line4, "use * for pre page");
            }
        }
        else if (is_admin == 0)
        {
            LCD_Write_String("1.see information");
            LCD_Write_Line(line2, "2.rotate motor");
            LCD_Write_Line(line3, "3.logout");
        }

        while (true)
        {
            char *key = key_scan();

            if (is_admin == 0)
            {
                if (strcmp(key, "1") == 0)
                {
                    state = 2;
                    get_info_page();
                    state = 1;
                    menu_page_num = -1;
                    break;
                }
                else if (strcmp(key, "2") == 0)
                {
                    rotate_motor_page();
                    menu_page_num = -1;
                    break;
                }
                else if (strcmp(key, "3") == 0)
                {
                    logout();
                    menu_page_num = -1;
                    break;
                }
            }
            else if (is_admin == 1)
            {
                if (strcmp(key, "#") == 0)
                {
                    menu_page_num = 1;
                    break;
                }
                if (strcmp(key, "*") == 0)
                {
                    menu_page_num = 0;
                    break;
                }

                if (menu_page_num == 0)
                {
                    if (strcmp(key, "1") == 0)
                    {
                        state = 2;
                        get_info_page();
                        state = 1;
                        menu_page_num = -1;
                        break;
                    }
                    else if (strcmp(key, "2") == 0)
                    {
                        rotate_motor_page();
                        menu_page_num = -1;
                        break;
                    }
                    else if (strcmp(key, "3") == 0)
                    {
                        date_setting_page();
                        menu_page_num = -1;
                        break;
                    }
                }
                else
                {
                    if (strcmp(key, "1") == 0)
                    {
                        add_user_page();
                        menu_page_num = -1;
                        break;
                    }
                    else if (strcmp(key, "2") == 0)
                    {
                        remove_user_page();
                        menu_page_num = -1;
                        break;
                    }
                    else if (strcmp(key, "3") == 0)
                    {
                        logout();
                        menu_page_num = -1;
                        break;
                    }
                }
            }
        }
    }
}
void logout()
{
    LCD_Timing_Write("Logged out ...");
    is_admin = -1;
    state = 0;
}
void add_user_page()
{
    char id[5] = "";
    char password[5] = "";

    LCD_Clear();

    {
        LCD_Write_String("Enter user id:");
        char *id_scaned = input(line2);
        sprintf(id, "%s", id_scaned);
    }
    {
        LCD_Write_Line(line3, "Enter user password:");
        char *password_scaned = input(line4);
        sprintf(password, "%s", password_scaned);
    }
    bool isEdited = false;

    LCD_Clear();
    for (char i = 0; i < users_count; i++)
    {
        if (strcmp(users[i].id, id) == 0)
        {
            strcpy(users[i].password, password);
            isEdited = true;
            LCD_Timing_Write("user updated ...");
            break;
        }
    }
    if (!isEdited)
    {
        if (users_count == max_user)
        {
            LCD_Clear();
            LCD_Write_String("The ram is Full");
            LCD_Write_Line(line2, "overwrite it?");
            LCD_Write_Line(line3, "(1=yes/0=no)");
            while (true)
            {
                char *key = key_scan();

                if (strcmp(key, "1") == 0)
                {
                    add_user(id, password);
                    LCD_Timing_Write("user added ...");
                    break;
                }
                if (strcmp(key, "0") == 0)
                {
                    LCD_Timing_Write("user dont add ...");
                    break;
                }
            }
        }
        else
        {
            add_user(id, password);
            LCD_Timing_Write("user added ...");
        }
    }
}
void add_user(char *id, char *password)
{
    if (users_count == max_user)
    {
        strcpy(users[users_count - 1].id, id);
        strcpy(users[users_count - 1].password, password);
    }
    else
    {
        strcpy(users[users_count].id, id);
        strcpy(users[users_count].password, password);
        users_count++;
    }
}
void remove_user_page()
{
    char id[5] = "";

    LCD_Clear();
    {
        LCD_Write_String("Enter user id:");
        char *id_scaned = input(line2);
        sprintf(id, "%s", id_scaned);
    }

    bool isEdited = false;

    LCD_Clear();
    for (char i = 0; i < users_count; i++)
    {
        if (strcmp(users[i].id, id) == 0)
        {
            users_count--;

            for (char k = i; k < users_count; k++)
            {
                users[k] = users[k + 1];
            }

            isEdited = true;
            break;
        }
    }

    if (isEdited)
    {
        LCD_Timing_Write("user deleted ...");
    }
    else
    {
        LCD_Timing_Write("the id not valid");
    }
}
