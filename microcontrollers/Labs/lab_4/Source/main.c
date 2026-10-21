
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Приклад: день народження 24
#define BIRTH_DAY 24

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

void main(void)
{
    unsigned char tens = BIRTH_DAY / 10;
    unsigned char units = BIRTH_DAY % 10;

    // Налаштування портів
    TRISA = 0x00;
    TRISB = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;

    while (1)
    {
        // Перший розряд
        PORTA = 0x00;
        PORTB = digits[tens];

        PORTAbits.RA2 = 1;
        __delay_ms(10);
        PORTAbits.RA2 = 0;

        // Другий розряд
        PORTB = digits[units];

        PORTAbits.RA3 = 1;
        __delay_ms(10);
        PORTAbits.RA3 = 0;
    }
}
дал
