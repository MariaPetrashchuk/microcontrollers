
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

#define PWM_OUT PORTBbits.RB0
#define BTN_UP  PORTBbits.RB1
#define BTN_DOWN PORTBbits.RB2

unsigned char duty = 50;

void main(void)
{
    unsigned char i;

    PORTB = 0x00;
    TRISB = 0b00000110;

    while (1)
    {
        // Збільшення коефіцієнта заповнення
        if (BTN_UP == 0)
        {
            if (duty < 90)
                duty += 10;

            __delay_ms(20);
            while (BTN_UP == 0);
        }

        // Зменшення коефіцієнта заповнення
        if (BTN_DOWN == 0)
        {
            if (duty > 10)
                duty -= 10;

            __delay_ms(20);
            while (BTN_DOWN == 0);
        }

        // Формування ШІМ-сигналу
        for (i = 0; i < 100; i++)
        {
            if (i < duty)
                PWM_OUT = 1;
            else
                PWM_OUT = 0;

            __delay_us(2);
        }
    }
}
