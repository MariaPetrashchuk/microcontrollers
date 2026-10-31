
#include <xc.h>

// ========================================
// PIC16F877A - LCD + Matrix Keypad 4x4
// ========================================

// Configuration bits
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

// Keypad symbols
const char keypad[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// ========================================
// Port initialization
// ========================================

void ports_init(void)
{
    ADCON1 = 0x06;

    // LCD control
    TRISB = 0xF8;
    PORTB = 0x00;

    // LCD data bus
    TRISD = 0x00;
    PORTD = 0x00;

    // RC0-RC3: outputs (rows)
    // RC4-RC7: inputs (columns)
    TRISC = 0xF0;
    PORTC = 0x0F;
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

// ========================================
// Keypad scanning
// ========================================

char keypad_scan(void)
{
    unsigned char row;
    unsigned char col;
    unsigned char columns;

    for (row = 0; row < 4; row++)
    {
        // All rows HIGH
        PORTC = 0x0F;

        // Set selected row LOW
        PORTC &= (unsigned char)~(1U << row);

        __delay_us(50);

        // Read columns RC4-RC7
        columns = (PORTC >> 4) & 0x0F;

        for (col = 0; col < 4; col++)
        {
            if ((columns & (1U << col)) == 0)
            {
                __delay_ms(20);

                columns = (PORTC >> 4) & 0x0F;

                if ((columns & (1U << col)) == 0)
                {
                    return keypad[row][col];
                }
            }
        }
    }

    return '\0';
}

// ========================================
// Main program
// ========================================

void main(void)
{
    char key;
    unsigned char position = 0;

    ports_init();
    lcd_init();

    lcd_set_cursor(1, 0);
    lcd_print("KEYPAD TEST");

    lcd_set_cursor(2, 0);

    while (1)
    {
        key = keypad_scan();

        if (key != '\0')
        {
            // Clear second row when full
            if (position >= 16)
            {
                lcd_set_cursor(2, 0);
                lcd_print("                ");
                position = 0;
            }

            lcd_set_cursor(2, position);
            lcd_data(key);

            position++;

            // Wait for key release
            do
            {
                __delay_ms(10);
            }
            while (keypad_scan() != '\0');

            __delay_ms(20);
        }
    }
}
