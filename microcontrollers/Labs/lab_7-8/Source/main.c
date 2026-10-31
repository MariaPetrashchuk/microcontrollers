
#include <xc.h>

// Тактова частота мікроконтролера
#define _XTAL_FREQ 4000000UL

// Конфігурація PIC16F877A
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

void main(void)
{
    // Налаштування портів буде додано
    // на наступному етапі

    while (1)
    {
        // Основний цикл програми
    }
}
