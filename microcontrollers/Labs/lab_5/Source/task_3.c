
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

#define TMR0_PRELOAD 61

void main(void)
{
    // Вимкнення переривань
    INTCON = 0x00;

    // Налаштування портів
    TRISA = 0x00;
    TRISB = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;

    // TMR0: внутрішнє тактування
    // Попередній дільник 1:256
    OPTION_REG = 0x87;

    while (1)
    {
        // Очистити прапорець переповнення
        INTCONbits.T0IF = 0;

        // Завантажити початкове значення
        TMR0 = TMR0_PRELOAD;

        // Очікувати переповнення TMR0
        while (INTCONbits.T0IF == 0)
        {
            // Очікування
        }

        // Перемикання RB0 для контролю
        PORTBbits.RB0 = !PORTBbits.RB0;
    }
}
