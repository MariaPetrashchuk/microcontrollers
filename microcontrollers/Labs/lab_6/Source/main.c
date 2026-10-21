
#include <xc.h>

// Конфігурація мікроконтролера PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Початкове значення TMR0
#define TMR0_PRELOAD 100

// Коди цифр для семисегментного
// індикатора зі спільним катодом
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

// Збереження стану PORTA
volatile unsigned char porta_state = 0;

// Обробник переривань TMR0
void __interrupt() ISR(void)
{
    if (INTCONbits.T0IE && INTCONbits.T0IF)
    {
        // Повторне завантаження таймера
        TMR0 = TMR0_PRELOAD;

        // Очищення прапорця переповнення
        INTCONbits.T0IF = 0;

        // Вимкнення обох розрядів
        // RA1 при цьому не змінюється
        porta_state &= 0xF3;
        PORTA = porta_state;

        if (active_digit == 0)
        {
            // Перший розряд: цифра 0
            PORTB = digits[0];

            // Увімкнення першого розряду
            porta_state |= 0x04;

            active_digit = 1;
        }
        else
        {
            // Другий розряд: цифра 3
            PORTB = digits[3];

            // Увімкнення другого розряду
            porta_state |= 0x08;

            active_digit = 0;
        }

        // Оновлення стану PORTA
        PORTA = porta_state;
    }
}

void main(void)
{
    // Початкове вимкнення виходів
    PORTA = 0x00;
    PORTB = 0x00;

    // Налаштування портів на вихід
    TRISA = 0x00;
    TRISB = 0x00;

    // Початковий стан PORTA
    porta_state = 0x00;

    // TMR0: внутрішнє тактування
    // Попередній дільник 1:64
    OPTION_REG = 0x85;

    // Початкове значення таймера
    TMR0 = TMR0_PRELOAD;

    // Налаштування переривань
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.GIE = 1;

    while (1)
    {
        // Увімкнення світлодіода RA1
        INTCONbits.GIE = 0;

        porta_state |= 0x02;
        PORTA = porta_state;

        INTCONbits.GIE = 1;

        // Затримка 500 мс
        __delay_ms(500);

        // Вимкнення світлодіода RA1
        INTCONbits.GIE = 0;

        porta_state &= 0xFD;
        PORTA = porta_state;

        INTCONbits.GIE = 1;

        // Затримка 500 мс
        __delay_ms(500);
    }
}
