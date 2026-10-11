#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF

void main(void)
{
    TRISA = 0b00011111;
    TRISB = 0x00;

    PORTB = 0x00;

    while (1)
    {
        if (PORTAbits.RA0 == 0)
        {
            if (PORTAbits.RA2 == 0)
                PORTBbits.RB1 = 1;
            else
                PORTB = 0x3C;
        }
        else
        {
            if (PORTAbits.RA2 == 0)
                PORTB = 129;
            else
                PORTB = 0xFF;
        }
    }
}
