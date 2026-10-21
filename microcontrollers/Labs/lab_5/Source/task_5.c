
#include <xc.h>

// Конфігурація PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Число для відображення
#define FIRST_DIGIT  0
#define SECOND_DIGIT 3

// Початкове значення таймера
#define TMR0_PRELOAD 100

// Коди цифр для індикатора
// зі спільним катодом
const unsigned char digits[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// Поточний активний розряд
volatile unsigned char active_digit = 0;

// Обробник переривань TMR0
void __interrupt() ISR(void)
{
    if (INTCONbits.T0IF && INTCONbits.T0IE)
    {
        // Повторне завантаження таймера
        TMR0 = TMR0_PRELOAD;

        // Скидання прапорця переповнення
        INTCONbits.T0IF = 0;

        // Вимкнення обох розрядів
        PORTA = 0x00;

        if (active_digit == 0)
        {
            // Виведення цифри 0
            PORTB = digits[FIRST_DIGIT];

            // Увімкнення першого розряду
            PORTA = 0x04;

            active_digit = 1;
        }
        else
        {
            // Виведення цифри 3
            PORTB = digits[SECOND_DIGIT];

            // Увімкнення другого розряду
            PORTA = 0x08;

            active_digit = 0;
        }
    }
}

void main(void)
{
    // Початкове налаштування портів
    PORTA = 0x00;
    PORTB = 0x00;

    TRISA = 0x00;
    TRISB = 0x00;

    // TMR0: внутрішнє джерело
    // Попередній дільник 1:64
    OPTION_REG = 0x85;

    // Початкове значення TMR0
    TMR0 = TMR0_PRELOAD;

    // Налаштування переривань
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.GIE = 1;

    while (1)
    {
        // Основна програма виконується
        // незалежно від оновлення індикатора
    }
}
