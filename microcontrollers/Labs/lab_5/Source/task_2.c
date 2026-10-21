
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Очікування одного переповнення TMR0
void wait_overflow(void)
{
    INTCONbits.T0IF = 0;

    while (INTCONbits.T0IF == 0)
    {
        // Очікування переповнення
    }
}

// Формування заданого інтервалу
// за кількістю машинних циклів
void timer_delay(unsigned long cycles)
{
    unsigned long remaining = cycles;

    while (remaining > 0)
    {
        unsigned int count;

        if (remaining >= 256UL)
        {
            count = 256;
        }
        else
        {
            count = (unsigned int)remaining;
        }

        // Початкове значення таймера
        TMR0 = (unsigned char)(256 - count);

        // Очікування переповнення
        wait_overflow();

        remaining -= count;
    }
}

void main(void)
{
    // Вимкнення переривань
    INTCON = 0x00;

    // Налаштування портів
    TRISA = 0x00;
    TRISB = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;

    // Внутрішнє тактування TMR0
    // Без попереднього дільника
    OPTION_REG = 0x88;

    while (1)
    {
        // Високий рівень — 20 мс
        PORTBbits.RB0 = 1;
        timer_delay(20000UL);

        // Низький рівень — 200 мс
        PORTBbits.RB0 = 0;
        timer_delay(200000UL);
    }
}
