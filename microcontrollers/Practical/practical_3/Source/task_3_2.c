
#include <xc.h>

// Конфігурація PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Один повний цикл TMR0 = 256 мкс
// без попереднього дільника
void wait_overflow(void)
{
    INTCONbits.T0IF = 0;

    while (INTCONbits.T0IF == 0)
    {
        // Очікування переповнення
    }
}

// Затримка приблизно 20 мс
void delay_20ms(void)
{
    unsigned char i;

    for (i = 0; i < 78; i++)
    {
        wait_overflow();
    }
}

// Затримка приблизно 200 мс
void delay_200ms(void)
{
    unsigned int i;

    for (i = 0; i < 781; i++)
    {
        wait_overflow();
    }
}

void main(void)
{
    // RB0 - вихід
    TRISB = 0xFE;
    PORTB = 0x00;

    // TMR0: внутрішнє джерело,
    // без попереднього дільника
    OPTION_REG = 0x08;

    // Переривання вимкнені
    INTCON = 0x00;

    // Початкове значення таймера
    TMR0 = 0;

    while (1)
    {
        // Високий рівень 20 мс
        PORTBbits.RB0 = 1;
        delay_20ms();

        // Низький рівень 200 мс
        PORTBbits.RB0 = 0;
        delay_200ms();
    }
}
