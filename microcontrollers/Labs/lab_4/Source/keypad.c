
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Таблиця кодів клавіш
const unsigned char keys[4][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {14, 0, 15}
};

// Функція сканування клавіатури
unsigned char scan_keypad(void)
{
    unsigned char col;
    unsigned char row;
    unsigned char state;

    for (col = 0; col < 3; col++)
    {
        // Активація одного стовпця
        PORTB = (1 << col);

        __delay_us(100);

        // Зчитування стану рядків
        state = (PORTB >> 4) & 0x0F;

        for (row = 0; row < 4; row++)
        {
            if (state & (1 << row))
            {
                // Усунення брязкоту контактів
                __delay_ms(20);

                // Повторна перевірка
                if (PORTB & (1 << (row + 4)))
                {
                    // Очікування відпускання кнопки
                    while (PORTB & (1 << (row + 4)))
                    {
                    }

                    __delay_ms(20);

                    return keys[row][col];
                }
            }
        }
    }

    return 0xFF;
}

void main(void)
{
    unsigned char key;

    // PORTA — виходи для індикатора
    TRISA = 0x00;

    // RB0–RB3 — виходи
    // RB4–RB7 — входи
    TRISB = 0xF0;

    PORTA = 0x00;
    PORTB = 0x00;

    while (1)
    {
        key = scan_keypad();

        if (key != 0xFF)
        {
            // Передавання коду клавіші
            PORTA = key & 0x0F;
        }
    }
}
