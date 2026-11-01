
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

volatile uint16_t adc_result = 0;

// Initialize ADC and ports
void ADC_Init(void)
{
    // RA0/AN0 is analog input
    TRISA = 0x01;

    // PORTB and PORTC are outputs
    TRISB = 0x00;
    TRISC = 0x00;

    PORTB = 0x00;
    PORTC = 0x00;

    // Right-justified result, AN0 analog,
    // remaining analog pins digital, Vref = VDD/VSS
    ADCON1 = 0x8E;

    // ADC clock Fosc/32, channel AN0, ADC ON
    ADCON0 = 0x81;

    __delay_us(40);
}

// Read 10-bit ADC value
uint16_t ADC_Read(void)
{
    // Acquisition time
    __delay_us(40);

    // Start conversion
    ADCON0bits.GO_nDONE = 1;

    // Wait until conversion finishes
    while (ADCON0bits.GO_nDONE);

    // Clear ADC interrupt flag
    PIR1bits.ADIF = 0;

    // Combine high and low bytes
    return ((uint16_t)ADRESH << 8) | ADRESL;
}

void main(void)
{
    ADC_Init();

    while (1)
    {
        adc_result = ADC_Read();

        // Lower 8 bits to PORTB
        PORTB = (uint8_t)(adc_result & 0xFF);

        // Upper 2 bits to RC0 and RC1
        PORTC = (uint8_t)((adc_result >> 8) & 0x03);

        __delay_ms(40);
    }
}

