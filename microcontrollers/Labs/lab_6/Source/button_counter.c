
#include <xc.h>

// Конфігурація PIC16F84A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

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

// Кількість натискань
volatile unsigned char counter = 0;

// Десятки та одиниці
volatile unsigned char tens = 0;
volatile unsigned char units = 0;

// Поточний розряд
volatile unsigned char active_digit = 0;

// Обробник переривань TMR0
void __interrupt() ISR(void)
{
    if (INTCONbits.T0IE && INTCONbits.T0IF)
    {
        // Перезавантаження таймера
        TMR0 = TMR0_PRELOAD;

        // Очищення прапорця
        INTCONbits.T0IF = 0;

        // Вимкнення обох розрядів
        PORTA = 0x00;

        if (active_digit == 0)
        {
            // Відображення десятків
            PORTB = digits[tens];

            // Увімкнення першого розряду
            PORTA = 0x04;

            active_digit = 1;
        }
        else
        {
            // Відображення одиниць
            PORTB = digits[units];

            // Увімкнення другого розряду
            PORTA = 0x08;

            active_digit = 0;
        }
    }
}

// Оновлення значення лічильника
void update_counter(void)
{
    counter++;

    if (counter > 99)
    {
        counter = 0;
    }

    // Розділення на десятки та одиниці
    tens = counter / 10;
    units = counter % 10;
}

void main(void)
{
    // RA0 — вхід для кнопки
    // RA2, RA3 — виходи індикатора
    TRISA = 0x01;

    // PORTB — виходи сегментів
    TRISB = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;

    // TMR0: внутрішнє тактування
    // Попередній дільник 1:64
    OPTION_REG = 0x85;

    TMR0 = TMR0_PRELOAD;

    // Дозвіл переривань
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.GIE = 1;

    while (1)
    {
        // Кнопка натиснута:
        // RA0 підключається до GND
        if (PORTAbits.RA0 == 0)
        {
            // Усунення брязкоту контактів
            __delay_ms(20);

            if (PORTAbits.RA0 == 0)
            {
                update_counter();

                // Очікування відпускання кнопки
                while (PORTAbits.RA0 == 0)
                {
                }

                __delay_ms(20);
            }
        }
    }
}
