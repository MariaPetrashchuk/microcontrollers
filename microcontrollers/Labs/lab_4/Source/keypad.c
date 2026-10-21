
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

const unsigned char keys[4][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {14, 0, 15}
};

unsigned char scan_keypad(void)
{
    unsigned char col;
    unsigned char row;
    unsigned char state;

    for (col = 0; col < 3; col++)
    {
        PORTB = (1 << col);
        __delay_us(100);

        state = (PORTB >> 4) & 0x0F;

        for (row = 0; row < 4; row++)
        {
            if (state & (1 << row))
            {
                __delay_ms(20);

                if (PORTB & (1 << (row + 4)))
                {
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

    TRISA = 0x00;
    TRISB = 0xF0;

    PORTA = 0x00;
    PORTB = 0x00;

    while (1)
    {
        key = scan_keypad();

        if (key != 0xFF)
        {
            PORTA = key & 0x0F;
        }
    }
}
