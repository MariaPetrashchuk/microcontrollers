
#include <xc.h>

// ========================================
// PIC16F877A - LCD MENU OPEN / CLOSE
// ========================================

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// LCD control pins
#define LCD_RS PORTBbits.RB0
#define LCD_RW PORTBbits.RB1
#define LCD_E  PORTBbits.RB2

// Buttons (active LOW)
#define BTN_UP   PORTCbits.RC0
#define BTN_DOWN PORTCbits.RC1
#define BTN_OK   PORTCbits.RC2
#define BTN_ESC  PORTCbits.RC3

// LED indicators
#define LED_GREEN PORTBbits.RB3
#define LED_RED   PORTBbits.RB4

// Menu states
#define START_SCREEN 0
#define MAIN_MENU    1
#define RESULT_SCREEN 2

unsigned char menu_state = START_SCREEN;
unsigned char selected_item = 0;
unsigned char FLAG = 0;

// ========================================
// Port initialization
// ========================================

void ports_init(void)
{
    ADCON1 = 0x06;

    // RB0-RB4 outputs
    TRISB = 0xE0;
    PORTB = 0x00;

    // LCD data bus
    TRISD = 0x00;
    PORTD = 0x00;

    // RC0-RC3 inputs
    TRISC = 0x0F;
    PORTC = 0x00;

    LED_GREEN = 0;
    LED_RED = 0;
}

// ========================================
// LCD functions
// ========================================

void lcd_write(unsigned char value,
               unsigned char rs)
{
    LCD_RS = rs;
    LCD_RW = 0;
    LCD_E = 0;

    PORTD = value;
    __delay_us(2);

    LCD_E = 1;
    __delay_us(2);
    LCD_E = 0;

    __delay_ms(2);
}

void lcd_command(unsigned char command)
{
    lcd_write(command, 0);
}

void lcd_data(unsigned char data)
{
    lcd_write(data, 1);
}

void lcd_init(void)
{
    __delay_ms(40);

    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E = 0;

    lcd_command(0x30);
    __delay_ms(5);

    lcd_command(0x30);
    __delay_us(150);

    lcd_command(0x30);
    lcd_command(0x38);

    lcd_command(0x08);
    lcd_command(0x01);
    lcd_command(0x06);
    lcd_command(0x0C);
}

void lcd_print(const char *text)
{
    while (*text != '\0')
    {
        lcd_data((unsigned char)*text);
        text++;
    }
}

void lcd_set_cursor(unsigned char row,
                    unsigned char column)
{
    if (row == 1)
    {
        lcd_command(0x80 + column);
    }
    else
    {
        lcd_command(0xC0 + column);
    }
}

void lcd_clear(void)
{
    lcd_command(0x01);
}

// ========================================
// Display screens
// ========================================

void show_start(void)
{
    lcd_clear();

    lcd_set_cursor(1, 0);
    lcd_print("LCD MENU SYSTEM");

    lcd_set_cursor(2, 0);
    lcd_print("PRESS OK");
}

void show_menu(void)
{
    lcd_clear();

    lcd_set_cursor(1, 0);

    if (selected_item == 0)
    {
        lcd_print(">OPEN");
    }
    else
    {
        lcd_print(" OPEN");
    }

    lcd_set_cursor(2, 0);

    if (selected_item == 1)
    {
        lcd_print(">CLOSE");
    }
    else
    {
        lcd_print(" CLOSE");
    }
}

void show_result(void)
{
    lcd_clear();

    lcd_set_cursor(1, 0);

    if (FLAG == 0)
    {
        lcd_print("OPENED");
    }
    else
    {
        lcd_print("CLOSED");
    }

    lcd_set_cursor(2, 0);
    lcd_print("ESC TO RETURN");
}

// ========================================
// Button processing
// ========================================

unsigned char button_pressed(void)
{
    // UP
    if (BTN_UP == 0)
    {
        __delay_ms(25);

        if (BTN_UP == 0)
        {
            while (BTN_UP == 0);
            __delay_ms(25);
            return 1;
        }
    }

    // DOWN
    if (BTN_DOWN == 0)
    {
        __delay_ms(25);

        if (BTN_DOWN == 0)
        {
            while (BTN_DOWN == 0);
            __delay_ms(25);
            return 2;
        }
    }

    // OK
    if (BTN_OK == 0)
    {
        __delay_ms(25);

        if (BTN_OK == 0)
        {
            while (BTN_OK == 0);
            __delay_ms(25);
            return 3;
        }
    }

    // ESC
    if (BTN_ESC == 0)
    {
        __delay_ms(25);

        if (BTN_ESC == 0)
        {
            while (BTN_ESC == 0);
            __delay_ms(25);
            return 4;
        }
    }

    return 0;
}

// ========================================
// Main program
// ========================================

void main(void)
{
    unsigned char button;

    ports_init();
    lcd_init();

    show_start();

    while (1)
    {
        button = button_pressed();

        if (button == 0)
        {
            continue;
        }

        // Start screen
        if (menu_state == START_SCREEN)
        {
            if (button == 3)
            {
                menu_state = MAIN_MENU;
                selected_item = 0;
                show_menu();
            }
        }

        // Main menu
        else if (menu_state == MAIN_MENU)
        {
            if (button == 1)
            {
                selected_item = 0;
                show_menu();
            }
            else if (button == 2)
            {
                selected_item = 1;
                show_menu();
            }
            else if (button == 3)
            {
                FLAG = selected_item;

                if (FLAG == 0)
                {
                    LED_GREEN = 1;
                    LED_RED = 0;
                }
                else
                {
                    LED_GREEN = 0;
                    LED_RED = 1;
                }

                menu_state = RESULT_SCREEN;
                show_result();
            }
            else if (button == 4)
            {
                menu_state = START_SCREEN;
                FLAG = 0;
                show_start();
            }
        }

        // Result screen
        else if (menu_state == RESULT_SCREEN)
        {
            if (button == 4)
            {
                FLAG = 0;
                menu_state = MAIN_MENU;
                show_menu();
            }
        }
    }
}
