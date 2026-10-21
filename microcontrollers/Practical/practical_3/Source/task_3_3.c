
#include <xc.h>

// Конфігурація PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Лічильник переповнень
volatile unsigned int overflow_count = 0;

// Ознака переповнення
volatile unsigned char overflow_detected = 0;

void main(void)
{
    // Внутрішнє джерело тактування
    // Дільник 1:256 підключений до TMR0
    OPTION_REG = 0x07;

    // Переривання вимкнені
    INTCON = 0x00;

    while (1)
    {
        overflow_detected = 0;

        // Початкове значення TMR0
        TMR0 = 61;

        // Скидання прапорця переповнення
        INTCONbits.T0IF = 0;

        // Очікування переповнення
        while (INTCONbits.T0IF == 0)
        {
            // Очікування
        }

        // Переповнення відбулося
        overflow_detected = 1;
        overflow_count++;

        // Скидання прапорця
        INTCONbits.T0IF = 0;
    }
}
