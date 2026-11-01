
#include "LCD.h"

// Generate Enable pulse
void zapys(void)
{
    LCD_E = 1;
    __delay_us(2);
    LCD_E = 0;
    __delay_ms(2);
}

// Send command to LCD
void lcd_com(uint8_t command)
{
    LCD_RS = 0;
    LCD_RW = 0;
    PORTD = command;
    zapys();
}

// Send ASCII character to LCD
void lcd_data(uint8_t data)
{
    LCD_RS = 1;
    LCD_RW = 0;
    PORTD = data;
    zapys();
}

// Initialize LCD in 8-bit mode
void lcd_init(void)
{
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;

    TRISD = 0x00;

    PORTBbits.RB0 = 0;
    PORTBbits.RB1 = 0;
    PORTBbits.RB2 = 0;
    PORTD = 0x00;

    __delay_ms(50);

    lcd_com(0x38); // 8-bit, 2 lines, 5x8
    lcd_com(0x0C); // Display ON, cursor OFF
    lcd_com(0x01); // Clear display
    lcd_com(0x06); // Increment cursor
}

// Display string
void lcd_string(const char *text)
{
    while (*text != '\0')
    {
        lcd_data((uint8_t)*text);
        text++;
    }
}

