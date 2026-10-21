
#include <xc.h>

// Конфігурація PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

volatile unsigned char overflow_detected = 0;
volatile unsigned int overflow_count = 0;

void main(void)
{
    // Внутрішнє тактування, без дільника
    OPTION_REG = 0x08;

    // Заборона переривань
    INTCON = 0x00;

    while (1)
    {
        overflow_detected = 0;

        // Початкове значення для 200 відліків
        // 256 - 200 - 2 = 54
        TMR0 = 54;

        // Скидання прапорця переповнення
        INTCONbits.T0IF = 0;

        // Очікування переповнення TMR0
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
