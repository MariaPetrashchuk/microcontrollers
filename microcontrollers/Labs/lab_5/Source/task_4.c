
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Лічильник переповнень
volatile unsigned char overflow_count = 0;

// Обробник переривань TMR0
void __interrupt() ISR(void)
{
    if (INTCONbits.T0IF && INTCONbits.T0IE)
    {
        // Очищення прапорця переповнення
        INTCONbits.T0IF = 0;

        overflow_count++;
    }
}

void main(void)
{
    // Налаштування портів
    TRISA = 0x00;
    TRISB = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;

    // Внутрішнє тактування TMR0
    // Попередній дільник 1:256
    OPTION_REG = 0x87;

    // Початкове значення таймера
    TMR0 = 0;

    // Налаштування переривань
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.GIE = 1;

    while (1)
    {
        // Очікування 7 повних переповнень
        overflow_count = 0;

        while (overflow_count < 7)
        {
            // Очікування
        }

        // Тимчасово заборонити переривання
        INTCONbits.GIE = 0;

        // Останній частковий цикл
        TMR0 = 95;
        INTCONbits.T0IF = 0;

        INTCONbits.GIE = 1;

        // Очікування восьмого переповнення
        while (INTCONbits.T0IF == 0)
        {
            // Очікування
        }

        // Перемикання RB0
        PORTBbits.RB0 = !PORTBbits.RB0;
    }
}
