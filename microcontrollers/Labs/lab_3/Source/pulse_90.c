
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

void main(void)
{
    // Налаштування PORTB на виведення
    TRISB = 0x00;
    PORTB = 0x00;

    while (1)
    {
        // Високий рівень на RB4
        PORTBbits.RB4 = 1;
        __delay_us(180);

        // Низький рівень на RB4
        PORTBbits.RB4 = 0;
        __delay_us(20);
    }
}
