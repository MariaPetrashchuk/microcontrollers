
#include <xc.h>
#include <stdint.h>

// ========================================
// LAB 11-12: USART / UART
// PIC16F877A, XC8, 4 MHz, 9600 baud
// ========================================

// 1 - Send name MARIIA
// 2 - Receive characters and Echo
// 3 - Forward data between two terminals
#define LAB_MODE 1

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// ========================================
// USART initialization
// ========================================

void uart_init(void)
{
    // RC6 = TX, RC7 = RX
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;

    // 9600 baud at 4 MHz:
    // BRGH = 1, SPBRG = 25
    // Actual baud rate = 9615.38
    SPBRG = 25;

    TXSTA = 0x24;  // TXEN=1, BRGH=1, SYNC=0
    RCSTA = 0x90;  // SPEN=1, CREN=1

    __delay_ms(100);
}

// ========================================
// Send one character
// ========================================

void uart_send_char(char data)
{
    while (PIR1bits.TXIF == 0)
    {
    }

    TXREG = data;
}

// ========================================
// Send text
// ========================================

void uart_send_text(const char *text)
{
    while (*text != '\0')
    {
        uart_send_char(*text);
        text++;
    }
}

// ========================================
// Receive one character
// ========================================

char uart_receive_char(void)
{
    // Recover from receiver overrun
    if (RCSTAbits.OERR)
    {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    while (PIR1bits.RCIF == 0)
    {
        if (RCSTAbits.OERR)
        {
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
    }

    // Discard a byte with framing error
    if (RCSTAbits.FERR)
    {
        volatile char discarded = RCREG;
        (void)discarded;
        return '\0';
    }

    return RCREG;
}

// ========================================
// MODE 1: Transmit name
// ========================================

void mode_send_name(void)
{
    while (1)
    {
        uart_send_text("MARIIA\r\n");
        __delay_ms(2000);
    }
}

// ========================================
// MODE 2: Receive and Echo
// ========================================

void mode_echo(void)
{
    char received;

    uart_send_text("USART ECHO READY\r\n");

    while (1)
    {
        received = uart_receive_char();

        if (received != '\0')
        {
            uart_send_char(received);
        }
    }
}

// ========================================
// MODE 3: Forward received data
// ========================================

void mode_forward(void)
{
    char received;

    while (1)
    {
        received = uart_receive_char();

        if (received != '\0')
        {
            uart_send_char(received);
        }
    }
}

// ========================================
// Main program
// ========================================

void main(void)
{
    uart_init();

#if LAB_MODE == 1

    mode_send_name();

#elif LAB_MODE == 2

    mode_echo();

#elif LAB_MODE == 3

    mode_forward();

#else

#error "LAB_MODE must be 1, 2 or 3"

#endif
}
