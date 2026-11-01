
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

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

// DS18B20 connected to RB7
#define OW_PIN  PORTBbits.RB7
#define OW_DIR  TRISBbits.TRISB7

volatile int16_t temperature_raw = 0;
volatile uint8_t temperature_low = 0;
volatile uint8_t temperature_high = 0;
volatile uint8_t sensor_present = 0;

// Open-drain emulation: never drive the bus HIGH
void OW_Low(void)
{
    OW_PIN = 0;
    OW_DIR = 0;
}

void OW_Release(void)
{
    OW_DIR = 1;
}

// Reset and presence detection
bool OW_Reset(void)
{
    bool present;

    OW_Low();
    __delay_us(480);

    OW_Release();
    __delay_us(70);

    present = (OW_PIN == 0);

    __delay_us(410);

    return present;
}

// Write one bit
void OW_WriteBit(uint8_t bit_value)
{
    OW_Low();

    if (bit_value)
    {
        __delay_us(6);
        OW_Release();
        __delay_us(64);
    }
    else
    {
        __delay_us(60);
        OW_Release();
        __delay_us(10);
    }
}

// Read one bit
uint8_t OW_ReadBit(void)
{
    uint8_t bit_value;

    OW_Low();
    __delay_us(3);

    OW_Release();
    __delay_us(12);

    bit_value = OW_PIN;

    __delay_us(55);

    return bit_value;
}

// Write byte, least significant bit first
void OW_WriteByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        OW_WriteBit(data & 0x01);
        data >>= 1;
    }
}

// Read byte, least significant bit first
uint8_t OW_ReadByte(void)
{
    uint8_t data = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        if (OW_ReadBit())
        {
            data |= (uint8_t)(1U << i);
        }
    }

    return data;
}

// Write TH, TL and configuration to SRAM
bool DS18B20_Configure(void)
{
    if (!OW_Reset())
    {
        return false;
    }

    OW_WriteByte(0xCC); // Skip ROM
    OW_WriteByte(0x4E); // Write Scratchpad

    OW_WriteByte(30);   // TH = +30 C
    OW_WriteByte(10);   // TL = +10 C
    OW_WriteByte(0x7F); // 12-bit resolution

    return true;
}

// Start temperature conversion
bool DS18B20_StartConversion(void)
{
    if (!OW_Reset())
    {
        return false;
    }

    OW_WriteByte(0xCC); // Skip ROM
    OW_WriteByte(0x44); // Convert T

    return true;
}

// Read temperature (raw 16-bit value)
bool DS18B20_ReadTemperature(int16_t *result)
{
    uint8_t low_byte;
    uint8_t high_byte;

    if (!OW_Reset())
    {
        return false;
    }

    OW_WriteByte(0xCC); // Skip ROM
    OW_WriteByte(0xBE); // Read Scratchpad

    low_byte = OW_ReadByte();
    high_byte = OW_ReadByte();

    temperature_low = low_byte;
    temperature_high = high_byte;

    *result = (int16_t)(
        ((uint16_t)high_byte << 8) | low_byte
    );

    return true;
}

void main(void)
{
    // PORTD displays the low temperature byte
    TRISD = 0x00;
    PORTD = 0x00;

    // Prepare RB7 for 1-Wire
    PORTBbits.RB7 = 0;
    OW_Release();

    __delay_ms(100);

    sensor_present = DS18B20_Configure();

    while (1)
    {
        if (DS18B20_StartConversion())
        {
            // Maximum conversion time at 12 bits
            __delay_ms(750);

            if (DS18B20_ReadTemperature(
                (int16_t *)&temperature_raw))
            {
                sensor_present = 1;

                // Display low byte on PORTD
                PORTD = temperature_low;
            }
            else
            {
                sensor_present = 0;
                PORTD = 0x00;
            }
        }
        else
        {
            sensor_present = 0;
            PORTD = 0x00;
        }

        __delay_ms(250);
    }
}

