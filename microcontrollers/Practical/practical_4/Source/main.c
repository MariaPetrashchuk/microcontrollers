
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Число для індикації — 03
#define DISPLAY_NUMBER 3

// Початкове значення TMR0
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

// Стан виходів PORTA
volatile unsigned char porta_state = 0;

// Обробник переривань
void __interrupt() ISR(void)
{
    if (INTCONbits.T0IF && INTCONbits.T0IE)
    {
        // Повторне завантаження таймера
        TMR0 = TMR0_PRELOAD;

        // Скидання прапорця переривання
        INTCONbits.T0IF = 0;

        // Вимкнення обох розрядів
        porta_state &= 0xF3;
        PORTA = porta_state;

        if (active_digit == 0)
        {
            // Відображення цифри 0
            PORTB = digits[0];

            // Увімкнення першого розряду
            porta_state |= 0x04;
            active_digit = 1;
        }
        else
        {
            // Відображення цифри 3
            PORTB = digits[DISPLAY_NUMBER];

            // Увімкнення другого розряду
            porta_state |= 0x08;
            active_digit = 0;
        }

        PORTA = porta_state;
    }
}

void main(void)
{
    // Налаштування портів
    TRISA = 0x00;
    TRISB = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;

    // Налаштування TMR0
    // Внутрішній тактовий сигнал
    // Попередній дільник 1:64
    OPTION_REG = 0x85;

    TMR0 = TMR0_PRELOAD;

    // Налаштування переривань
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.GIE = 1;

    // Основний цикл програми
    while (1)
    {
        // Увімкнення світлодіода
        INTCONbits.GIE = 0;
        porta_state |= 0x02;
        PORTA = porta_state;
        INTCONbits.GIE = 1;

        __delay_ms(500);

        // Вимкнення світлодіода
        INTCONbits.GIE = 0;
        porta_state &= 0xFD;
        PORTA = porta_state;
        INTCONbits.GIE = 1;

        __delay_ms(500);
    }
}
