
#include <xc.h>

// Конфігурація PIC16F84A — варіант 1
#pragma config FOSC = XT
#pragma config WDTE = ON
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Оголошення змінних
volatile unsigned char TMP1;
volatile unsigned char TMP2;
volatile unsigned char RESULT;

// Оголошення функції
unsigned char SUM(unsigned char a, unsigned char b);

// Опис функції
unsigned char SUM(unsigned char a, unsigned char b)
{
    return a + b;
}

// Основна програма
void main(void)
{
    // Ініціалізація змінних
    TMP1 = 10;
    TMP2 = 20;
    RESULT = 0;

    // Нескінченний робочий цикл
    while (1)
    {
        RESULT = SUM(TMP1, TMP2);

        // Скидання сторожового таймера
        CLRWDT();
    }
}
