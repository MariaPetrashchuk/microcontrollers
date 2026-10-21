
#include <xc.h>

// Конфігурація PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Лічильник інтервалів
volatile unsigned char interval_count = 0;

// Ознака завершення інтервалу 500 мс
volatile unsigned char interval_complete = 0;

// Очікування одного інтервалу ~50 мс
void delay_50ms(void)
{
    TMR0 = 61;

    // Скидання прапорця переповнення
    INTCONbits.T0IF = 0;

    // Очікування переповнення
    while (INTCONbits.T0IF == 0)
    {
        // Очікування
    }

    INTCONbits.T0IF = 0;
}

void main(void)
{
    // Налаштування RB0 як виходу
    TRISB = 0xFE;
    PORTB = 0x00;

    // Внутрішнє тактування TMR0
    // Дільник 1:256
    OPTION_REG = 0x07;

    // Вимкнення переривань
    INTCON = 0x00;

    while (1)
    {
        interval_complete = 0;
        interval_count = 0;

        // Формування інтервалу ~500 мс
        while (interval_count < 10)
        {
            delay_50ms();
            interval_count++;
        }

        // Інтервал завершено
        interval_complete = 1;

        // Змінюємо стан RB0
        PORTBbits.RB0 = !PORTBbits.RB0;
    }
}
