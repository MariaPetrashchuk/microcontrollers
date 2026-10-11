
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

#define LED_OK  PORTAbits.RA0
#define LED_ERR PORTAbits.RA1
#define RELAY   PORTAbits.RA2

#define MAX_LEN 4

// PIN-код: 1223
const unsigned char PIN_CODE[4] = {
    0x11, 0x22, 0x32, 0x34
};

unsigned char entered[MAX_LEN];
unsigned char count = 0;

void clear_code(void)
{
    count = 0;
    LED_OK = 0;
    LED_ERR = 0;
    RELAY = 0;
}

void add_key(unsigned char mask)
{
    if (count < MAX_LEN)
    {
        entered[count] =
            (unsigned char)(((count + 1) << 4) | mask);
        count++;
    }
}

unsigned char codes_match(void)
{
    unsigned char i;

    if (count != MAX_LEN)
        return 0;

    for (i = 0; i < MAX_LEN; i++)
    {
        if (entered[i] != PIN_CODE[i])
            return 0;
    }

    return 1;
}

void main(void)
{
    unsigned char key;

    PORTA = 0x00;
    TRISA = 0x00;
    TRISB = 0x3F;

    // Вимкнення виходів на початку
    clear_code();

    while (1)
    {
        // Кнопка CLR
        if (PORTBbits.RB5 == 0)
        {
            clear_code();

            while (PORTBbits.RB5 == 0);
            __delay_ms(20);
        }

        // Кнопка OK
        else if (PORTBbits.RB4 == 0)
        {
            if (codes_match())
            {
                LED_OK = 1;
                LED_ERR = 0;
                RELAY = 1;
            }
            else
            {
                LED_OK = 0;
                LED_ERR = 1;
                RELAY = 0;
            }

            while (PORTBbits.RB4 == 0);
            __delay_ms(20);
        }

        // Опитування клавіатури
        else
        {
            key = 0;

            if (PORTBbits.RB3 == 0)
                key = 1;
            else if (PORTBbits.RB2 == 0)
                key = 2;
            else if (PORTBbits.RB1 == 0)
                key = 4;
            else if (PORTBbits.RB0 == 0)
                key = 8;

            if (key != 0)
            {
                __delay_ms(20);

                // Перевірка, чи кнопка ще натиснута
                if ((PORTB & 0x0F) != 0x0F)
                {
                    add_key(key);

                    // Очікування відпускання кнопок
                    while ((PORTB & 0x0F) != 0x0F);

                    __delay_ms(20);
                }
            }
        }
    }
}
