
#ifndef LCD_H
#define LCD_H

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000UL

// LCD control pins
#define LCD_RS PORTBbits.RB0
#define LCD_RW PORTBbits.RB1
#define LCD_E  PORTBbits.RB2

// LCD data bus DB0-DB7 -> PORTD

void zapys(void);
void lcd_com(uint8_t command);
void lcd_data(uint8_t data);
void lcd_init(void);
void lcd_string(const char *text);

#endif

