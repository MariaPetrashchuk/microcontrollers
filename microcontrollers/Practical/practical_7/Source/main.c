
#include <xc.h>
#include <stdint.h>

// Configuration bits
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Initialize USART: 9600 baud, 8N1
void USART_Init(void)
{
    // RC6 = TX, RC7 = RX
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;

    // Baud rate: 4 MHz / (16 * (25 + 1))
    SPBRG = 25;

    // Asynchronous mode, BRGH = 1, TX enabled
    TXSTA = 0x24;

    // Serial port enabled, continuous reception
    RCSTA = 0x90;
}

// Send one character
void USART_Write(char data)
{
    while (!PIR1bits.TXIF)
    {
        // Wait until TXREG is available
    }

    TXREG = data;
}

// Send text string
void USART_WriteText(const char *text)
{
    while (*text != '\0')
    {
        USART_Write(*text);
        text++;
    }
}

// Receive one character
char USART_Read(void)
{
    // Recover from receive buffer overrun
    if (RCSTAbits.OERR)
    {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    while (!PIR1bits.RCIF)
    {
        // Wait for incoming character

        if (RCSTAbits.OERR)
        {
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
    }

    // Discard bytes with framing errors
    if (RCSTAbits.FERR)
    {
        volatile char discarded = RCREG;
        (void)discarded;
        return '\0';
    }

    return RCREG;
}

void main(void)
{
    char received;

    USART_Init();

    __delay_ms(500);

    // Send name using ASCII
    USART_WriteText("MARIIA\r\n");

    // Receive and forward terminal characters
    while (1)
    {
        received = USART_Read();

        if (received != '\0')
        {
            USART_Write(received);
        }
    }
}

