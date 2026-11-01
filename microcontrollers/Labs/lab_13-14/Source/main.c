
#include <xc.h>
#include <stdint.h>

// =======================================
// LAB 13-14: DS18B20 / 1-Wire
// PIC16F877A, XC8, 4 MHz
// =======================================

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

#define TH_VALUE      30
#define TL_VALUE      10
#define CONFIG_VALUE  0x7F

// 1-Wire data line: RB5
#define OW_PIN PORTBbits.RB5
#define OW_DIR TRISBbits.TRISB5

volatile uint8_t sensor_present = 0;
volatile uint8_t write_verified = 0;

volatile uint8_t read_th = 0;
volatile uint8_t read_tl = 0;
volatile uint8_t read_config = 0;

volatile int16_t temperature_raw = 0;

// =======================================
// 1-Wire bus control
// =======================================

void ow_low(void)
{
    PORTBbits.RB5 = 0;
    OW_DIR = 0;
}

void ow_release(void)
{
    OW_DIR = 1;
}

uint8_t ow_reset(void)
{
    uint8_t presence;

    ow_low();
    __delay_us(480);

    ow_release();
    __delay_us(70);

    presence = (OW_PIN == 0);

    __delay_us(410);

    return presence;
}

// =======================================
// Write one bit
// =======================================

void ow_write_bit(uint8_t bit_value)
{
    ow_low();

    if (bit_value)
    {
        __delay_us(6);
        ow_release();
        __delay_us(64);
    }
    else
    {
        __delay_us(60);
        ow_release();
        __delay_us(10);
    }
}

// =======================================
// Read one bit
// =======================================

uint8_t ow_read_bit(void)
{
    uint8_t result;

    ow_low();
    __delay_us(3);

    ow_release();
    __delay_us(10);

    result = OW_PIN;

    __delay_us(53);

    return result;
}

// =======================================
// Write / read byte (LSB first)
// =======================================

void ow_write_byte(uint8_t data)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        ow_write_bit(data & 0x01);
        data >>= 1;
    }
}

uint8_t ow_read_byte(void)
{
    uint8_t i;
    uint8_t data = 0;

    for (i = 0; i < 8; i++)
    {
        if (ow_read_bit())
        {
            data |= (uint8_t)(1U << i);
        }
    }

    return data;
}

// =======================================
// Write Scratchpad:
// TH, TL, configuration
// =======================================

uint8_t ds18b20_write_scratchpad(void)
{
    if (!ow_reset())
    {
        return 0;
    }

    ow_write_byte(0xCC); // Skip ROM
    ow_write_byte(0x4E); // Write Scratchpad

    ow_write_byte(TH_VALUE);
    ow_write_byte(TL_VALUE);
    ow_write_byte(CONFIG_VALUE);

    return 1;
}

// =======================================
// Read Scratchpad and verify
// =======================================

uint8_t ds18b20_verify_scratchpad(void)
{
    uint8_t temp_lsb;
    uint8_t temp_msb;

    if (!ow_reset())
    {
        return 0;
    }

    ow_write_byte(0xCC); // Skip ROM
    ow_write_byte(0xBE); // Read Scratchpad

    temp_lsb = ow_read_byte();
    temp_msb = ow_read_byte();

    read_th = ow_read_byte();
    read_tl = ow_read_byte();
    read_config = ow_read_byte();

    (void)temp_lsb;
    (void)temp_msb;

    return (read_th == TH_VALUE &&
            read_tl == TL_VALUE &&
            read_config == CONFIG_VALUE);
}

// =======================================
// Start temperature conversion
// =======================================

uint8_t ds18b20_start_conversion(void)
{
    if (!ow_reset())
    {
        return 0;
    }

    ow_write_byte(0xCC); // Skip ROM
    ow_write_byte(0x44); // Convert T

    return 1;
}

// =======================================
// Read temperature
// =======================================

uint8_t ds18b20_read_temperature(void)
{
    uint8_t lsb;
    uint8_t msb;

    if (!ow_reset())
    {
        return 0;
    }

    ow_write_byte(0xCC); // Skip ROM
    ow_write_byte(0xBE); // Read Scratchpad

    lsb = ow_read_byte();
    msb = ow_read_byte();

    temperature_raw = (int16_t)(
        ((uint16_t)msb << 8) | lsb
    );

    return 1;
}

// =======================================
// Main
// =======================================

void main(void)
{
    ADCON1 = 0x06;

    PORTB = 0x00;
    TRISB = 0xFF;

    ow_release();
    __delay_ms(100);

    while (1)
    {
        sensor_present = ow_reset();

        if (sensor_present)
        {
            ds18b20_write_scratchpad();

            write_verified =
                ds18b20_verify_scratchpad();

            if (ds18b20_start_conversion())
            {
                // 12-bit conversion: max 750 ms
                __delay_ms(750);

                ds18b20_read_temperature();
            }
        }
        else
        {
            write_verified = 0;
        }

        __delay_ms(1000);
    }
}
