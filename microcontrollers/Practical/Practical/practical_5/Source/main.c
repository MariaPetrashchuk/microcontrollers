
#include <xc.h>
#include "LCD.h"

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

void main(void)
{
    ADCON1 = 0x06;

    lcd_init();

    // First line
    lcd_com(0x80);
    lcd_string("MARIIA");

    // Second line
    lcd_com(0xC0);
    lcd_string("PETRASHCHUK");

    while (1)
    {
        // Keep text on LCD
    }
}

