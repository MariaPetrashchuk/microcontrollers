
#include <xc.h>

// Конфігурація PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

void main(void)
{
    // Вимкнення всіх переривань
    INTCON = 0x00;

    // Налаштування портів
    TRISA = 0x00;
    TRISB = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;

    // TMR0 працює від внутрішнього джерела
    // PSA = 1: дільник призначений WDT
    // T0CS = 0: внутрішній тактовий сигнал
    OPTION_REG = 0x88;

    while (1)
    {
        // Очистити прапорець переповнення
        INTCONbits.T0IF = 0;

        // Початкове значення для 200 імпульсів
        TMR0 = 54;

        // Очікування переповнення таймера
        while (INTCONbits.T0IF == 0)
        {
            // Очікування
        }

        // Переповнення відбулося
        // Змінюємо стан RB0 для контролю
        PORTBbits.RB0 = !PORTBbits.RB0;
    }
}
