
#include <xc.h>

// Конфігурація мікроконтролера PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

void main(void)
{
    // Налаштування портів
    TRISA = 0x1F;  // PORTA - введення
    TRISB = 0x00;  // PORTB - виведення

    PORTB = 0x00;

    while (1)
    {
        // K1 підключена до RA0
        // K2 підключена до RA1

        if (PORTAbits.RA0 == 0)
        {
            // K1 натиснута
            if (PORTAbits.RA1 == 0)
            {
                // Обидві кнопки натиснуті
                PORTBbits.RB1 = 1;
            }
            else
            {
                // K1 натиснута, K2 не натиснута
                PORTB = 0x3C;
            }
        }
        else
        {
            // K1 не натиснута
            if (PORTAbits.RA1 == 0)
            {
                // K2 натиснута
                PORTB = 129;
            }
            else
            {
                // Обидві кнопки не натиснуті
                PORTB = 0xFF;
            }
        }

        __delay_ms(20);
    }
}
